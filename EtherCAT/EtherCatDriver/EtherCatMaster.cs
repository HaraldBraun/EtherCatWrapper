using System.Net.NetworkInformation;
using System.Text;

namespace EtherCatDriver;

public enum EcState: int {
    None = 0x00,
    Init = 0x01,
    PreOp = 0x02,
    Boot = 0x03,
    SafeOp = 0x04,
    Operational = 0x08
}

public class EtherCatMaster {
    public int Init( string ifname ) => NativeMethods.ECW_Init( ifname );
    public int ConfigInit( ) => NativeMethods.ECW_ConfigInit( );
    public void Close( ) => NativeMethods.ECW_Close( );
    public int GetSlaveCount( ) => NativeMethods.ECW_GetSlaveCount( );

    public bool TryGetSlaveVendorProduct( int slaveIdx, out uint vendor, out uint product ) {
        int result = NativeMethods.ECW_GetSlaveVendorProduct(slaveIdx, out vendor, out product);
        return result == 1;
    }

    
    public EcState SetSlaveState( int slaveIdx, EcState reqState, int timeoutUs ) =>
        // The native ECW_GetSlaveState call already masks the state byte internally 
        // with "& 0x0F" before returning the value (see C wrapper). 
        // This is necessary because SOEM provides the actual state value in the event of an error 
        // additionally bit 0x10 (EC_STATE_ACK / EC_STATE_ERROR) is added via OR 
        // (e.g. 0x12 = PRE_OP with error flag instead of pure 0x02). 
        // The direct cast to EcState is therefore only safe as long as this masking 
        // persists in the C wrapper. If it is removed or changed there, 
        // this cast must be adjusted accordingly - otherwise invalid 
        // EcState values are created. Check error states separately using HasSlaveError().
        (EcState) NativeMethods.ECW_SetSlaveState( slaveIdx, (int) reqState, timeoutUs );

    public EcState GetSlaveState( int slaveIdx ) =>
        (EcState) NativeMethods.ECW_GetSlaveState( slaveIdx );

    public bool HasSlaveError( int slaveIdx ) => NativeMethods.ECW_HasSlaveError( slaveIdx ) == 1;

    public bool TryReadEeprom( int slaveIdx, ushort eepromAddr, out ushort data ) {
        int result = NativeMethods.ECW_ReadEeprom(slaveIdx, eepromAddr, out data);
        return result == 1;
    }

    public bool WriteEeprom( int slaveIdx, ushort eepromAddr, ushort data ) =>
        NativeMethods.ECW_WriteEeprom( slaveIdx, eepromAddr, data ) == 1;

    public bool UpdateEepromCrc( int slaveIdx ) => NativeMethods.ECW_UpdateEepromCrc( slaveIdx ) == 1;

    public bool WriteEepromWithCrc( int slaveIdx, ushort eepromAddr, ushort data ) =>
        NativeMethods.ECW_WriteEepromWithCrc( slaveIdx, eepromAddr, data ) == 1;

    public int GetAdapterCount( ) => NativeMethods.ECW_GetAdapterCount( );

    public string? GetAdapterName( int idx ) {
        var buffer = new byte[256];
        int result = NativeMethods.ECW_GetAdapterName(idx, buffer, buffer.Length);
        if (result != 1) return null;

        int nullIndex = Array.IndexOf(buffer, (byte)0);
        return Encoding.UTF8.GetString( buffer, 0, nullIndex >= 0 ? nullIndex : buffer.Length );
    }

    public int FindWorkingAdapter( ) => NativeMethods.ECW_FindWorkingAdapter( );
}