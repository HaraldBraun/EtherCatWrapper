//using System.Net.NetworkInformation;
using System.Text;

namespace EtherCatDriver;

public enum EcState : int
{
    None = 0x00,
    Init = 0x01,
    PreOp = 0x02,
    Boot = 0x03,
    SafeOp = 0x04,
    Operational = 0x08
}

public class EtherCatMaster
{
    public int Init(string ifname) => NativeMethods.ECW_Init(ifname);
    public int ConfigInit() => NativeMethods.ECW_ConfigInit();
    public void Close() => NativeMethods.ECW_Close();
    public int GetSlaveCount() => NativeMethods.ECW_GetSlaveCount();

    public bool TryGetSlaveVendorProduct(int slaveIdx, out uint vendor, out uint product)
    {
        int result = NativeMethods.ECW_GetSlaveVendorProduct(slaveIdx, out vendor, out product);
        return result == 1;
    }
    
    public EcState SetSlaveState(int slaveIdx, EcState reqState, int timeoutUs) =>
        // The native ECW_GetSlaveState call already masks the state byte internally 
        // with "& 0x0F" before returning the value (see C wrapper). 
        // This is necessary because SOEM provides the actual state value in the event of an error 
        // additionally bit 0x10 (EC_STATE_ACK / EC_STATE_ERROR) is added via OR 
        // (e.g. 0x12 = PRE_OP with error flag instead of pure 0x02). 
        // The direct cast to EcState is therefore only safe as long as this masking 
        // persists in the C wrapper. If it is removed or changed there, 
        // this cast must be adjusted accordingly - otherwise invalid 
        // EcState values are created. Check error states separately using HasSlaveError().
        (EcState)NativeMethods.ECW_SetSlaveState(slaveIdx, (int)reqState, timeoutUs);

    public EcState GetSlaveState(int slaveIdx) =>
        (EcState)NativeMethods.ECW_GetSlaveState(slaveIdx);

    public bool HasSlaveError(int slaveIdx) => NativeMethods.ECW_HasSlaveError(slaveIdx) == 1;

    public bool TryReadEeprom(int slaveIdx, ushort eepromAddr, out ushort data)
    {
        int result = NativeMethods.ECW_ReadEeprom(slaveIdx, eepromAddr, out data);
        return result == 1;
    }

    public bool WriteEeprom(int slaveIdx, ushort eepromAddr, ushort data) =>
        NativeMethods.ECW_WriteEeprom(slaveIdx, eepromAddr, data) == 1;

    public bool UpdateEepromCrc(int slaveIdx) => NativeMethods.ECW_UpdateEepromCrc(slaveIdx) == 1;

    public bool WriteEepromWithCrc(int slaveIdx, ushort eepromAddr, ushort data) =>
        NativeMethods.ECW_WriteEepromWithCrc(slaveIdx, eepromAddr, data) == 1;

    public int GetAdapterCount() => NativeMethods.ECW_GetAdapterCount();

    public string? GetAdapterName(int idx)
    {
        var buffer = new byte[256];
        int result = NativeMethods.ECW_GetAdapterName(idx, buffer, buffer.Length);
        if (result != 1) return null;

        int nullIndex = Array.IndexOf(buffer, (byte)0);
        return Encoding.UTF8.GetString(buffer, 0, nullIndex >= 0 ? nullIndex : buffer.Length);
    }

    public int FindWorkingAdapter() => NativeMethods.ECW_FindWorkingAdapter();

    public bool TryReadSlaveAlias(int slaveIdx, out ushort slaveAlias)
    {
        int result = NativeMethods.ECW_GetSlaveAlias(slaveIdx, out slaveAlias);
        return result == 1;
    }

    public string GetAssemblyLocation()
    {
        return typeof(EtherCatMaster).Assembly.Location;
    }

    // *************************
    // ReadSdo Sektion
    // *************************
    public bool TryReadSdoBool(int slaveIdx, ushort index, byte subIndex, out bool value)
    {
        int result = NativeMethods.ECW_ReadSdoBool(slaveIdx, index, subIndex, out value);
        return result > 0;
    }

    public bool TryReadSdoUInt8(int slaveIdx, ushort index, byte subIndex, out byte value)
    {
        int result = NativeMethods.ECW_ReadSdoUInt8(slaveIdx, index, subIndex, out value);
        return result > 0;
    }

    public bool TryReadSdoUInt16(int slaveIdx, ushort index, byte subIndex, out ushort value)
    {
        int result = NativeMethods.ECW_ReadSdoUInt16(slaveIdx, index, subIndex, out value);
        return result > 0;
    }

    public bool TryReadSdoUInt32(int slaveIdx, ushort index, byte subIndex, out uint value)
    {
        int result = NativeMethods.ECW_ReadSdoUInt32(slaveIdx, index, subIndex, out value);
        return result > 0;
    }

    public bool TryReadSdoUInt64(int slaveIdx, ushort index, byte subIndex, out ulong value)
    {
        int result = NativeMethods.ECW_ReadSdoUInt64(slaveIdx, index, subIndex, out value);
        return result > 0;
    }

    public bool TryReadSdoInt8(int slaveIdx, ushort index, byte subIndex, out sbyte value)
    {
        int result = NativeMethods.ECW_ReadSdoInt8(slaveIdx, index, subIndex, out value);
        return result > 0;
    }

    public bool TryReadSdoInt16(int slaveIdx, ushort index, byte subIndex, out short value)
    {
        int result = NativeMethods.ECW_ReadSdoInt16(slaveIdx, index, subIndex, out value);
        return result > 0;
    }

    public bool TryReadSdoInt32(int slaveIdx, ushort index, byte subIndex, out int value)
    {
        int result = NativeMethods.ECW_ReadSdoInt32(slaveIdx, index, subIndex, out value);
        return result > 0;
    }

    public bool TryReadSdoInt64(int slaveIdx, ushort index, byte subIndex, out long value)
    {
        int result = NativeMethods.ECW_ReadSdoInt64(slaveIdx, index, subIndex, out value);
        return result > 0;
    }

    public bool TryReadSdoFloat(int slaveIdx, ushort index, byte subIndex, out float value)
    {
        int result = NativeMethods.ECW_ReadSdoFloat(slaveIdx, index, subIndex, out value);
        return result > 0;
    }

    public bool TryReadSdoDouble(int slaveIdx, ushort index, byte subIndex, out double value)
    {
        int result = NativeMethods.ECW_ReadSdoDouble(slaveIdx, index, subIndex, out value);
        return result > 0;
    }

    public bool TryReadSdoString(int slaveIdx, ushort index, byte subIndex, out string? value)
    {
        var buffer = new byte[256];

        int result = NativeMethods.ECW_ReadSdoString(
            slaveIdx,
            index,
            subIndex,
            buffer,
            buffer.Length);

        if (result <= 0)
        {
            value = null;
            return false;
        }

        int nullIndex = Array.IndexOf(buffer, (byte)0);

        value = Encoding.UTF8.GetString(
            buffer,
            0,
            nullIndex >= 0 ? nullIndex : buffer.Length);

        return true;
    }

    // *************************
    // WriteSdo Sektion
    // *************************
    public bool WriteSdoBool( int slaveIdx, ushort index, byte subIndex, bool value ) =>
        NativeMethods.ECW_WriteSdoBool( slaveIdx, index, subIndex, value ) > 0;

    public bool WriteSdoUInt8( int slaveIdx, ushort index, byte subIndex, byte value ) =>
        NativeMethods.ECW_WriteSdoUInt8( slaveIdx, index, subIndex, value ) > 0;

    public bool WriteSdoUInt16( int slaveIdx, ushort index, byte subIndex, ushort value ) =>
        NativeMethods.ECW_WriteSdoUInt16( slaveIdx, index, subIndex, value ) > 0;

    public bool WriteSdoUInt32( int slaveIdx, ushort index, byte subIndex, uint value ) =>
        NativeMethods.ECW_WriteSdoUInt32( slaveIdx, index, subIndex, value ) > 0;

    public bool WriteSdoUInt64( int slaveIdx, ushort index, byte subIndex, ulong value ) =>
        NativeMethods.ECW_WriteSdoUInt64( slaveIdx, index, subIndex, value ) > 0;

    public bool WriteSdoInt8( int slaveIdx, ushort index, byte subIndex, sbyte value ) =>
        NativeMethods.ECW_WriteSdoInt8( slaveIdx, index, subIndex, value ) > 0;

    public bool WriteSdoInt16( int slaveIdx, ushort index, byte subIndex, short value ) =>
        NativeMethods.ECW_WriteSdoInt16( slaveIdx, index, subIndex, value ) > 0;

    public bool WriteSdoInt32( int slaveIdx, ushort index, byte subIndex, int value ) =>
        NativeMethods.ECW_WriteSdoInt32( slaveIdx, index, subIndex, value ) > 0;

    public bool WriteSdoInt64( int slaveIdx, ushort index, byte subIndex, long value ) =>
        NativeMethods.ECW_WriteSdoInt64( slaveIdx, index, subIndex, value ) > 0;

    public bool WriteSdoFloat( int slaveIdx, ushort index, byte subIndex, float value ) =>
        NativeMethods.ECW_WriteSdoFloat( slaveIdx, index, subIndex, value ) > 0;

    public bool WriteSdoDouble( int slaveIdx, ushort index, byte subIndex, double value ) =>
        NativeMethods.ECW_WriteSdoDouble( slaveIdx, index, subIndex, value ) > 0;

    public bool WriteSdoString( int slaveIdx, ushort index, byte subIndex, string value ) {
        byte[] buffer = Encoding.UTF8.GetBytes(value);
        return NativeMethods.ECW_WriteSdoString( slaveIdx, index, subIndex, buffer, buffer.Length ) > 0;
    }
}