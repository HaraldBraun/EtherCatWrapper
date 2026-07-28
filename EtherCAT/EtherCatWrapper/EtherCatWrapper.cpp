#include "pch.h"
#include "EtherCatWrapper.h"
#include "..\res\soem\soem.h"

static ecx_contextt g_ctx;
static bool g_initialized = false;
static pcap_if_t *g_adapterList = nullptr;

static unsigned char SiiCrc8(const unsigned char *data, int len)
{
   unsigned char crc = 0xFF;
   for (int i = 0; i < len; i++)
   {
      crc ^= data[i];
      for (int bit = 0; bit < 8; bit++)
      {
         if (crc & 0x80)
            crc = (unsigned char)((crc << 1) ^ 0x07);
         else
            crc = (unsigned char)(crc << 1);
      }
   }
   return crc;
}

int ECW_Init(const char *ifname)
{
   // ec_init öffnet den Netzwerkadapter für Raw-Ethernet-Zugriff (via Npcap)
   if (ecx_init(&g_ctx, ifname))
   {
      g_initialized = true;
      return 1;
   }
   g_initialized = false;
   return 0;
}

int ECW_ConfigInit()
{
   if (!g_initialized)
   {
      return -1;
   }

   // Scannt den Bus, bringt alle Slaves nach PRE-OP und füllt ec_slave[]
   int slaveCount = ecx_config_init(&g_ctx);
   return slaveCount;
}

void ECW_Close()
{
   if (g_initialized)
   {
      ecx_close(&g_ctx);
      g_initialized = false;
   }
}

int ECW_GetSlaveCount()
{
    if (!g_initialized)
    {
       return -1;
    }

    //g_ctx.slavecount is filled by ecx_config_init()
    return g_ctx.slavecount;
}

int ECW_GetSlaveVendorProduct(int slaveIdx, unsigned int* outVendor, unsigned int* outProduct)
{
    if (!g_initialized || outVendor == nullptr || outProduct == nullptr)
    {
       return 0;
    }

    // SOEM is 1-based: slavelist[0] is reserved for the master,
    // real slaves come up at index 1
    if (slaveIdx < 1 || slaveIdx > g_ctx.slavecount)
    {
       return 0;
    }

    // eep_man / eep_id are read from the SII EEPROM of the slave during config init
    *outVendor = g_ctx.slavelist[slaveIdx].eep_man;
    *outProduct = g_ctx.slavelist[slaveIdx].eep_id;

    return 1;
}

int ECW_SetSlaveState(int slaveIdx, ec_state reqState, int timeoutUs)
{
   if (!g_initialized || slaveIdx < 1 || slaveIdx > g_ctx.slavecount)
   {
      return 0;
   }

   g_ctx.slavelist[slaveIdx].state = reqState;
   ecx_writestate(&g_ctx, slaveIdx);

   return ecx_statecheck(&g_ctx, slaveIdx, reqState, timeoutUs);
}

int ECW_GetSlaveState(int slaveIdx)
{
   if (!g_initialized || slaveIdx < 1 || slaveIdx > g_ctx.slavecount)
   {
      return 0;
   }

   ecx_readstate(&g_ctx);

   // Mask the error/ack flag (0x10) to isolate the raw state.
   return g_ctx.slavelist[slaveIdx].state & 0x0F;
}

// Separate function to specifically check if the slave is in an error state.
int ECW_HasSlaveError(int slaveIdx)
{
   if (!g_initialized || slaveIdx < 1 || slaveIdx > g_ctx.slavecount)
   {
      return 0;
   }

   ecx_readstate(&g_ctx);

   return (g_ctx.slavelist[slaveIdx].state & EC_STATE_ERROR) ? 1 : 0;
}

int ECW_ReadEeprom(int slaveIdx, unsigned short eepromAddr, unsigned short *outData)
{
   if (!g_initialized || outData == nullptr)
   {
      return 0;
   }

   if (slaveIdx < 1 || slaveIdx > g_ctx.slavecount)
   {
      return 0;
   }

   uint16 configadr = g_ctx.slavelist[slaveIdx].configadr;

   // FPWR/FPRD method: addresses the slave via its fixed configuration address
   // instead of auto-increment – ​​more reliable with an already configured bus
   uint64 result = ecx_readeepromFP(&g_ctx, configadr, eepromAddr, EC_TIMEOUTEEP);

   *outData = (unsigned short)(result & 0xFFFF);

   return 1;
}

int ECW_WriteEeprom(int slaveIdx, unsigned short eepromAddr, unsigned short data)
{
   if (!g_initialized)
   {
      return 0;
   }

   if (slaveIdx < 1 || slaveIdx > g_ctx.slavecount)
   {
      return 0;
   }

   uint16 configadr = g_ctx.slavelist[slaveIdx].configadr;

   int wkc = ecx_writeeepromFP(&g_ctx, configadr, eepromAddr, data, EC_TIMEOUTEEP);

   return (wkc > 0) ? 1 : 0;
}

int ECW_UpdateEepromCrc(int slaveIdx)
{
   if (!g_initialized || slaveIdx < 1 || slaveIdx > g_ctx.slavecount)
   {
      return 0;
   }

   // The first 7 words (0x00–0x06) = 14 bytes form the CRC basis.
   unsigned char buf[14];
   for (int word = 0; word <= 0x06; word++)
   {
      unsigned short value;
      if (!ECW_ReadEeprom(slaveIdx, (unsigned short)word, &value))
      {
         return 0;
      }
      buf[word * 2] = (unsigned char)(value & 0xFF);            // Low-Byte
      buf[word * 2 + 1] = (unsigned char)((value >> 8) & 0xFF); // High-Byte
   }

   unsigned char crc = SiiCrc8(buf, 14);

   // Word 0x07: Low byte = CRC, high byte is reserved – DO NOT simply overwrite, 
   // but retain the existing value there.
   unsigned short oldWord07;
   if (!ECW_ReadEeprom(slaveIdx, 0x07, &oldWord07))
   {
      return 0;
   }

   unsigned short newWord07 = (oldWord07 & 0xFF00) | crc;

   return ECW_WriteEeprom(slaveIdx, 0x07, newWord07);
}

int ECW_WriteEepromWithCrc(int slaveIdx, unsigned short eepromAddr, unsigned short data)
{
   if (!ECW_WriteEeprom(slaveIdx, eepromAddr, data))
   {
      return 0;
   }

   return ECW_UpdateEepromCrc(slaveIdx);
}

int ECW_GetAdapterCount()
{
   // Vorherige Liste freigeben, falls schon mal abgefragt wurde
   if (g_adapterList != nullptr)
   {
      pcap_freealldevs(g_adapterList);
      g_adapterList = nullptr;
   }

   char errbuf[PCAP_ERRBUF_SIZE];
   if (pcap_findalldevs(&g_adapterList, errbuf) == -1)
   {
      return -1;
   }

   int count = 0;
   for (pcap_if_t *d = g_adapterList; d != nullptr; d = d->next)
   {
      count++;
   }
   return count;
}

int ECW_GetAdapterName(int idx, char *buffer, int bufferSize)
{
   if (g_adapterList == nullptr || buffer == nullptr || bufferSize <= 0)
   {
      return 0;
   }

   int i = 0;
   for (pcap_if_t *d = g_adapterList; d != nullptr; d = d->next, i++)
   {
      if (i == idx)
      {
         strncpy_s(buffer, bufferSize, d->name, _TRUNCATE);
         return 1;
      }
   }
   return 0;
}

int ECW_FindWorkingAdapter()
{
   int count = ECW_GetAdapterCount();
   if (count <= 0)
   {
      return -1;
   }

   int i = 0;
   for (pcap_if_t *d = g_adapterList; d != nullptr; d = d->next, i++)
   {
      if (ECW_Init(d->name))
      {
         int slaveCount = ECW_ConfigInit();
         if (slaveCount > 0)
         {
            // Erfolg – Zustand bleibt initialisiert, direkt weiterarbeitbar
            return i;
         }
         ECW_Close();
      }
   }
   return -1;
}