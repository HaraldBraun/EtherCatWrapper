#pragma once
#include "..\res\soem\soem.h"

#ifdef ETHERCATWRAPPER_EXPORTS
#define ECW_API extern "C" __declspec(dllexport)
#else
#define ECW_API extern "C" __declspec(dllimport)
#endif

// Initializes SOEM on the specified network adapter.
// ifname: Name of the network adapter, e.g., "\\Device\\NPF_{GUID}"
// Return value: 1 = Success, 0 = Error
ECW_API int ECW_Init(const char *ifname);

// Returns the number of slaves found after a successful ec_config_init.
// Returns the number of slaves, or - 1 if ECW_Init was unsuccessful.
ECW_API int ECW_ConfigInit();

// Releases all OEM resources.
ECW_API void ECW_Close();

// Count of slaves found at ConfigInit
ECW_API int ECW_GetSlaveCount();

// Delivers Vendor-ID and Product Code from a slave (1-based index, as in SOEM usual)
// Return: 1 = Pass, 0 = invalid index
ECW_API int ECW_GetSlaveVendorProduct(int slaveIdx, unsigned int* outVendor, unsigned int* outProduct);

// Brings the specified slave to the desired state (e.g., PRE-OP)
// reqState: OEM state constant (e.g., EC_STATE_PRE_OP)
// timeoutUs: Timeout in microseconds for state_check
// Returns: actual state reached, or 0 on error
ECW_API int ECW_SetSlaveState(int slaveIdx, ec_state reqState, int timeoutUs);

// Reads the current state of a slave (without changing it)
ECW_API int ECW_GetSlaveState(int slaveIdx);

// Separate function to specifically check if the slave is in an error state.
ECW_API int ECW_HasSlaveError(int slaveIdx);

// Reads a 16-bit word from the slave's SII EEPROM
// eepromAddr: Word address (not byte address!)
// Return value: 1 = Success, 0 = Error; value is written to outData
ECW_API int ECW_ReadEeprom(int slaveIdx, unsigned short eepromAddr, unsigned short* outData);

// Writes a 16 - bit word to the slave's SII EEPROM – WITHOUT CRC update
ECW_API int ECW_WriteEeprom(int slaveIdx, unsigned short eepromAddr, unsigned short data);

// Reads the relevant EEPROM area, recalculates the SII-CRC8, and writes it back
ECW_API int ECW_UpdateEepromCrc(int slaveIdx);

// Convenience function : ECW_WriteEeprom + ECW_UpdateEepromCrc in one call
ECW_API int ECW_WriteEepromWithCrc(int slaveIdx, unsigned short eepromAddr, unsigned short data);

// Returns the number of network adapters found
ECW_API int ECW_GetAdapterCount();

// Returns the name of the adapter at index idx (0-based)
// buffer: buffer provided by the caller, bufferSize: its size
ECW_API int ECW_GetAdapterName(int idx, char *buffer, int bufferSize);

// Try Init+ConfigInit on all detected adapters,
// until one responds with at least one slave
// Return: Index of the working adapter, or -1 if none match
ECW_API int ECW_FindWorkingAdapter();