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

// Read slave alias directly from struct
ECW_API int ECW_GetSlaveAlias(int slaveIdx, unsigned short *alias);

// *************************
// * Section for ECW_ReadSdo
// *************************

// Reads a UINT8 object from the CoE object dictionary.
ECW_API int ECW_ReadSdoUInt8(int slaveIdx, uint16_t index, uint8_t subIndex, uint8_t *value);

// Reads a UINT16 object from the CoE object dictionary.
ECW_API int ECW_ReadSdoUInt16(int slaveIdx, uint16_t index, uint8_t subIndex, uint16_t *value);

// Reads a UINT32 object from the CoE object dictionary.
ECW_API int ECW_ReadSdoUInt32(int slaveIdx, uint16_t index, uint8_t subIndex, uint32_t *value);

// Reads a UINT64 object from the CoE object dictionary.
ECW_API int ECW_ReadSdoUInt64(int slaveIdx, uint16_t index, uint8_t subIndex, uint64_t *value);

// Reads an INT8 object from the CoE object dictionary.
ECW_API int ECW_ReadSdoInt8(int slaveIdx, uint16_t index, uint8_t subIndex, int8_t *value);

// Reads an INT16 object from the CoE object dictionary.
ECW_API int ECW_ReadSdoInt16(int slaveIdx, uint16_t index, uint8_t subIndex, int16_t *value);

// Reads an INT32 object from the CoE object dictionary.
ECW_API int ECW_ReadSdoInt32(int slaveIdx, uint16_t index, uint8_t subIndex, int32_t *value);

// Reads an INT64 object from the CoE object dictionary.
ECW_API int ECW_ReadSdoInt64(int slaveIdx, uint16_t index, uint8_t subIndex, int64_t *value);

// Reads a BOOL object from the CoE object dictionary.
ECW_API int ECW_ReadSdoBool(int slaveIdx, uint16_t index, uint8_t subIndex, bool *value);

// Reads a FLOAT object from the CoE object dictionary.
ECW_API int ECW_ReadSdoFloat(int slaveIdx, uint16_t index, uint8_t subIndex, float *value);

// Reads a DOUBLE object from the CoE object dictionary.
ECW_API int ECW_ReadSdoDouble(int slaveIdx, uint16_t index, uint8_t subIndex, double *value);

// Reads a null-terminated string object from the CoE object dictionary.
// Reads a CoE Visible String object.
// The caller must provide a writable buffer.
ECW_API int ECW_ReadSdoString(int slaveIdx, uint16_t index, uint8_t subIndex, char *buffer, int bufferSize);

// *************************
// * Section for ECW_WriteSdo
// *************************

// Writes a BOOL object to the CoE object dictionary.
ECW_API int ECW_WriteSdoBool(int slaveIdx, uint16_t index, uint8_t subIndex, bool value);

// Writes a UINT8 object to the CoE object dictionary.
ECW_API int ECW_WriteSdoUInt8(int slaveIdx, uint16_t index, uint8_t subIndex, uint8_t value);

// Writes a UINT16 object to the CoE object dictionary.
ECW_API int ECW_WriteSdoUInt16(int slaveIdx, uint16_t index, uint8_t subIndex, uint16_t value);

// Writes a UINT32 object to the CoE object dictionary.
ECW_API int ECW_WriteSdoUInt32(int slaveIdx, uint16_t index, uint8_t subIndex, uint32_t value);

// Writes a UINT64 object to the CoE object dictionary.
ECW_API int ECW_WriteSdoUInt64(int slaveIdx, uint16_t index, uint8_t subIndex, uint64_t value);

// Writes an INT8 object to the CoE object dictionary.
ECW_API int ECW_WriteSdoInt8(int slaveIdx, uint16_t index, uint8_t subIndex, int8_t value);

// Writes an INT16 object to the CoE object dictionary.
ECW_API int ECW_WriteSdoInt16(int slaveIdx, uint16_t index, uint8_t subIndex, int16_t value);

// Writes an INT32 object to the CoE object dictionary.
ECW_API int ECW_WriteSdoInt32(int slaveIdx, uint16_t index, uint8_t subIndex, int32_t value);

// Writes an INT64 object to the CoE object dictionary.
ECW_API int ECW_WriteSdoInt64(int slaveIdx, uint16_t index, uint8_t subIndex, int64_t value);

// Writes a FLOAT object to the CoE object dictionary.
ECW_API int ECW_WriteSdoFloat(int slaveIdx, uint16_t index, uint8_t subIndex, float value);

// Writes a DOUBLE object to the CoE object dictionary.
ECW_API int ECW_WriteSdoDouble(int slaveIdx, uint16_t index, uint8_t subIndex, double value);

// Writes a CoE Visible String object.
// buffer: caller-provided data, length: exact number of bytes to write (no implicit null terminator added).
ECW_API int ECW_WriteSdoString(int slaveIdx, uint16_t index, uint8_t subIndex, const char *buffer, int length);

// *************************
// Debug Section
// *************************
ECW_API unsigned short ECW_DebugGetAlias(int slaveIdx);
ECW_API unsigned short ECW_DebugGetConfigAdr(int slaveIdx);
ECW_API const char *ECW_DebugGetName(int slaveIdx);
ECW_API unsigned int ECW_DebugGetRevision(int slaveIdx);
ECW_API unsigned int ECW_DebugGetSerial(int slaveIdx);
ECW_API int ECW_DebugReadSdoString(int slaveIdx, uint16_t index, uint8_t subindex, char *buffer, int bufferSize);
