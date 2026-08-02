using System.Runtime.InteropServices;
using System.Runtime.InteropServices.Marshalling;

namespace EtherCatDriver;

internal static partial class NativeMethods {
    private const string DllName = "EtherCatWrapper.dll";

    [LibraryImport( DllName, StringMarshalling = StringMarshalling.Utf8 )]
    public static partial int ECW_Init( string ifname );

    [LibraryImport( DllName )]
    public static partial int ECW_ConfigInit( );

    [LibraryImport( DllName )]
    public static partial void ECW_Close( );

    [LibraryImport( DllName )]
    public static partial int ECW_GetSlaveCount( );

    [LibraryImport( DllName )]
    public static partial int ECW_GetSlaveVendorProduct( int slaveIdx, out uint outVendor, out uint outProduct );

    [LibraryImport( DllName )]
    public static partial int ECW_SetSlaveState( int slaveIdx, int reqState, int timeoutUs );

    [LibraryImport( DllName )]
    public static partial int ECW_GetSlaveState( int slaveIdx );

    [LibraryImport( DllName )]
    public static partial int ECW_HasSlaveError( int slaveIdx );

    [LibraryImport( DllName )]
    public static partial int ECW_ReadEeprom( int slaveIdx, ushort eepromAddr, out ushort outData );

    [LibraryImport( DllName )]
    public static partial int ECW_WriteEeprom( int slaveIdx, ushort eepromAddr, ushort data );

    [LibraryImport( DllName )]
    public static partial int ECW_UpdateEepromCrc( int slaveIdx );

    [LibraryImport( DllName )]
    public static partial int ECW_WriteEepromWithCrc( int slaveIdx, ushort eepromAddr, ushort data );

    [LibraryImport( DllName )]
    public static partial int ECW_GetAdapterCount( );

    [LibraryImport( DllName )]
    public static partial int ECW_GetAdapterName( int idx, byte[] buffer, int bufferSize );

    [LibraryImport( DllName )]
    public static partial int ECW_FindWorkingAdapter( );
}