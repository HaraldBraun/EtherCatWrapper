using System.Runtime.InteropServices;

namespace EtherCatDriver;

internal static class NativeMethods {
    private const string DllName = "EtherCatWrapper.dll";

    [DllImport( DllName, CharSet = CharSet.Ansi)]
    public static extern int ECW_Init( string ifname );

    [DllImport( DllName )]
    public static extern int ECW_ConfigInit( );

    [DllImport( DllName )]
    public static extern void ECW_Close( );

    [DllImport( DllName )]
    public static extern int ECW_GetSlaveCount( );

    [DllImport( DllName )]
    public static extern int ECW_GetSlaveVendorProduct( int slaveIdx, out uint outVendor, out uint outProduct );
    
    [DllImport( DllName )]
    public static extern int ECW_SetSlaveState( int slaveIdx, int reqState, int timeoutUs );

    [DllImport( DllName )]
    public static extern int ECW_GetSlaveState( int slaveIdx );

    [DllImport( DllName )]
    public static extern int ECW_HasSlaveError( int slaveIdx );

    [DllImport( DllName )]
    public static extern int ECW_ReadEeprom( int slaveIdx, ushort eepromAddr, out ushort outData );

    [DllImport( DllName )]
    public static extern int ECW_WriteEeprom( int slaveIdx, ushort eepromAddr, ushort data );

    [DllImport( DllName )]
    public static extern int ECW_UpdateEepromCrc( int slaveIdx );

    [DllImport( DllName )]
    public static extern int ECW_WriteEepromWithCrc( int slaveIdx, ushort eepromAddr, ushort data );

    [DllImport( DllName )]
    public static extern int ECW_GetAdapterCount( );

    [DllImport( DllName )]
    public static extern int ECW_GetAdapterName( int idx, byte[] buffer, int bufferSize );

    [DllImport( DllName )]
    public static extern int ECW_FindWorkingAdapter( );

    [DllImport( DllName )]
    public static extern int ECW_GetSlaveAlias( int slaveIdx, out ushort slaveAlias );

    // *************************
    // ReadSdo Sektion
    // *************************
    [DllImport(DllName)] 
    internal static extern int ECW_ReadSdoBool(int slaveIdx, ushort index, byte subIndex, [MarshalAs(UnmanagedType.I1)] out bool value);

    [DllImport(DllName)] 
    internal static extern int ECW_ReadSdoUInt8(int slaveIdx, ushort index, byte subIndex, out byte value);

    [DllImport(DllName)] 
    internal static extern int ECW_ReadSdoUInt16(int slaveIdx, ushort index, byte subIndex, out ushort value);

    [DllImport(DllName)] 
    internal static extern int ECW_ReadSdoUInt32(int slaveIdx, ushort index, byte subIndex, out uint value);

    [DllImport(DllName)] 
    internal static extern int ECW_ReadSdoUInt64(int slaveIdx, ushort index, byte subIndex, out ulong value);

    [DllImport(DllName)] 
    internal static extern int ECW_ReadSdoInt8(int slaveIdx, ushort index, byte subIndex, out sbyte value);

    [DllImport(DllName)] 
    internal static extern int ECW_ReadSdoInt16(int slaveIdx, ushort index, byte subIndex, out short value);

    [DllImport(DllName)] 
    internal static extern int ECW_ReadSdoInt32(int slaveIdx, ushort index, byte subIndex, out int value);

    [DllImport(DllName)] 
    internal static extern int ECW_ReadSdoInt64(int slaveIdx, ushort index, byte subIndex, out long value);

    [DllImport(DllName)] 
    internal static extern int ECW_ReadSdoFloat(int slaveIdx, ushort index, byte subIndex, out float value);

    [DllImport(DllName)] 
    internal static extern int ECW_ReadSdoDouble(int slaveIdx, ushort index, byte subIndex, out double value);

    [DllImport(DllName)] 
    internal static extern int ECW_ReadSdoString(int slaveIdx, ushort index, byte subIndex, byte[] buffer, int bufferSize);

    // *************************
    // WriteSdo Sektion
    // *************************
    [DllImport( DllName )]
    internal static extern int ECW_WriteSdoBool( int slaveIdx, ushort index, byte subIndex, [MarshalAs( UnmanagedType.I1 )] bool value );

    [DllImport( DllName )]
    internal static extern int ECW_WriteSdoUInt8( int slaveIdx, ushort index, byte subIndex, byte value );

    [DllImport( DllName )]
    internal static extern int ECW_WriteSdoUInt16( int slaveIdx, ushort index, byte subIndex, ushort value );

    [DllImport( DllName )]
    internal static extern int ECW_WriteSdoUInt32( int slaveIdx, ushort index, byte subIndex, uint value );

    [DllImport( DllName )]
    internal static extern int ECW_WriteSdoUInt64( int slaveIdx, ushort index, byte subIndex, ulong value );

    [DllImport( DllName )]
    internal static extern int ECW_WriteSdoInt8( int slaveIdx, ushort index, byte subIndex, sbyte value );

    [DllImport( DllName )]
    internal static extern int ECW_WriteSdoInt16( int slaveIdx, ushort index, byte subIndex, short value );

    [DllImport( DllName )]
    internal static extern int ECW_WriteSdoInt32( int slaveIdx, ushort index, byte subIndex, int value );

    [DllImport( DllName )]
    internal static extern int ECW_WriteSdoInt64( int slaveIdx, ushort index, byte subIndex, long value );

    [DllImport( DllName )]
    internal static extern int ECW_WriteSdoFloat( int slaveIdx, ushort index, byte subIndex, float value );

    [DllImport( DllName )]
    internal static extern int ECW_WriteSdoDouble( int slaveIdx, ushort index, byte subIndex, double value );

    [DllImport( DllName )]
    internal static extern int ECW_WriteSdoString( int slaveIdx, ushort index, byte subIndex, byte[] buffer, int length );

}