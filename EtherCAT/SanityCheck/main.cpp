#include <iostream>
#include <cstdint>
#include "..\EtherCatWrapper\EtherCatWrapper.h" // replace with actual header

int main()
{
   std::cout << "Sanity check starting...\n";

   int found = ECW_FindWorkingAdapter();
   std::cout << "ECW_FindWorkingAdapter() returned: " << found << '\n';
   if (found == -1)
   {
      std::cerr << "No working adapter found\n";
      // return 1;
   }

   int slaveCount = ECW_GetSlaveCount();
   std::cout << "ECW_GetSlaveCount() = " << slaveCount << '\n';

   std::cout << "State before = " << ECW_GetSlaveState(1) << std::endl;

   ECW_SetSlaveState(1, EC_STATE_PRE_OP, 1000000);

   std::cout << "State after = " << ECW_GetSlaveState(1) << std::endl;

   unsigned short alias = ECW_DebugGetAlias(1);
   std::cout << "Alias = "
             << alias
             << " (0x"
             << std::hex
             << alias
             << std::dec
             << ")\n";

   unsigned short config = ECW_DebugGetConfigAdr(1);
   std::cout << "Config = "
             << config
             << " (0x"
             << std::hex
             << config
             << std::dec
             << ")\n";

   const char *name = ECW_DebugGetName(1);
   std::cout << "Name = "
             << name << "\n";

   unsigned int revision = ECW_DebugGetRevision(1);
   std::cout << "Revision = "
             << revision
             << " (0x"
             << std::hex
             << revision
             << std::dec
             << ")\n";

   unsigned int serial = ECW_DebugGetSerial(1);
   std::cout << "Serial = "
             << serial
             << " (0x"
             << std::hex
             << serial
             << std::dec
             << ")\n";

   uint32_t vendor = 0;
   uint32_t product = 0;
   int res = ECW_GetSlaveVendorProduct(1, &vendor, &product);
   std::cout << "ECW_GetSlaveVednorProduct(1, &vendor, &product) returned: " << res
             << ", Vendor = " << std::dec << vendor << std::hex
             << " (0x" << vendor << ")\n"
             << ", Product = " << std::dec << product << std::hex
             << "(0x" << product << ")\n";

   /* uint16_t val = 0;
   int rc = ECW_ReadEeprom(1, 0, &val);
   std::cout << "ECW_ReadEEprom(1, 0, &val) returned: " << rc
             << ", value = 0x" << std::hex << val << std::dec
             << " (" << val << ")\n";

   rc = ECW_ReadEeprom(1, 4, &val);
   std::cout << "ECW_ReadEEprom(1, 4, &val) returned: " << rc
             << ", value = 0x" << std::hex << val << std::dec
             << " (" << val << ")\n";
   rc = ECW_ReadEeprom(1, 5, &val);
   std::cout << "ECW_ReadEEprom(1, 5, &val) returned: " << rc
             << ", value = 0x" << std::hex << val << std::dec
             << " (" << val << ")\n";
   rc = ECW_ReadEeprom(1, 6, &val);
   std::cout << "ECW_ReadEEprom(1, 6, &val) returned: " << rc
             << ", value = 0x" << std::hex << val << std::dec
             << " (" << val << ")\n";
   rc = ECW_ReadEeprom(1, 18, &val);
   std::cout << "ECW_ReadEEprom(1, 18, &val) returned: " << rc
             << ", value = 0x" << std::hex << val << std::dec
             << " (" << val << ")\n";

   /* for (int i = 0; i < 256; i++)
   {
      uint16_t val = 0;

      if (ECW_ReadEeprom(1, i, &val))
      {
         if (val != 0)
         {
            std::cout << "ECW_ReadEeprom(1, " << i << ", &val): "
                      << "value = 0x" << std::hex << val << std::dec
                      << " (" << val << ")\n";
         }
      }
   }*/

   char buffer[256] = {0};

   int wkc = ECW_DebugReadSdoString(1, 0x1008, 0, buffer, sizeof(buffer));
   std::cout << "SDO 0x1008:0 WKC = "
             << wkc
             << std::endl;

   std::cout << "Value = "
             << buffer
             << std::endl;

   wkc = ECW_DebugReadSdoString(1, 0x1009, 0, buffer, sizeof(buffer));
   std::cout << "SDO 0x1009:0 WKC = "
             << wkc
             << std::endl;

   std::cout << "Value = "
             << buffer
             << std::endl;

   wkc = ECW_DebugReadSdoString(1, 0x100A, 0, buffer, sizeof(buffer));
   std::cout << "SDO 0x100A:0 WKC = "
             << wkc
             << std::endl;

   std::cout << "Value = "
             << buffer
             << std::endl;

   wkc = ECW_DebugReadSdoString(1, 0x100B, 0, buffer, sizeof(buffer));
   std::cout << "SDO 0x100B:0 WKC = "
             << wkc
             << std::endl;

   std::cout << "Value = "
             << buffer
             << std::endl;

   wkc = ECW_DebugReadSdoString(1, 0x100C, 0, buffer, sizeof(buffer));
   std::cout << "SDO 0x100C:0 WKC = "
             << wkc
             << std::endl;

   std::cout << "Value = "
             << buffer
             << std::endl;

   wkc = ECW_DebugReadSdoString(1, 0x100D, 0, buffer, sizeof(buffer));
   std::cout << "SDO 0x100D:0 WKC = "
             << wkc
             << std::endl;

   std::cout << "Value = "
             << buffer
             << std::endl;

   uint32_t value = 0;

   for (uint8_t sub = 0; sub <= 16; sub++)
   {
      int wkc = ECW_ReadSdoUInt32(1, 0x1018, sub, &value);

      std::cout << "0x1018:" << (int)sub << " WKC=" << wkc << " Value=" << value
               << " (0x" << std::hex << value << std::dec << ")" << std::endl;
   }

   for (uint16_t index = 0x2000; index <= 0x2022; index++)
   {
      uint32_t value = 0;

      int wkc = ECW_ReadSdoUInt32(1, index, 0, &value);

      if (wkc > 0)
      {
         std::cout << "Index 0x" << std::hex << index << std::dec << " = " << value << std::endl;
      }
   }

   return 0;
}