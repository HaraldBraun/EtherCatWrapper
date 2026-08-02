using System.Xml.Linq;

namespace EtherCatDriver;

public readonly struct EniSlaveInfo {
    public uint VendorId { get; }
    public uint ProductCode { get; }

    public EniSlaveInfo( uint vendorId, uint productCode ) {
        VendorId = vendorId;
        ProductCode = productCode;
    }
}

public static class EniParser {
    /// <summary>
    /// Extracts the VendorId and ProductCode of the first slave from an ENI file.
    /// </summary>
    /// <returns>EniSlaveInfo on success, null if the file is unreadable or invalid.</returns>
    public static EniSlaveInfo? ParseFirstSlave( string eniFilePath ) {
        try {
            var doc = XDocument.Load(eniFilePath);

            // Equivalent to root.find(".//Slave/Info") in the Python reference
            var info = doc.Descendants("Slave").Elements("Info").FirstOrDefault();
            if (info is null) {
                return null;
            }

            var vendorElement = info.Element("VendorId");
            var productElement = info.Element("ProductCode");
            if (vendorElement is null || productElement is null) {
                return null;
            }

            uint vendorId = (uint)vendorElement;
            uint productCode = (uint)productElement;

            return new EniSlaveInfo( vendorId, productCode );
        } catch (Exception) {
            // File missing, invalid XML, or values not parseable
            return null;
        }
    }

    /// <summary>
    /// Searches among the already-detected slaves (from EtherCatMaster) for the one
    /// that matches Vendor/Product from the ENI file.
    /// Fallback: first slave if none match exactly (same behavior as the Python reference).
    /// </summary>
    /// <returns>1-based slave index, or -1 if no slaves are present.</returns>
    public static int FindTargetSlaveIndex( EtherCatMaster master, string eniFilePath ) {
        int slaveCount = master.GetSlaveCount();
        if (slaveCount <= 0) {
            return -1;
        }

        var eniInfo = ParseFirstSlave(eniFilePath);
        if (eniInfo is null) {
            return -1;
        }

        for (int idx = 1; idx <= slaveCount; idx++) {
            if (master.TryGetSlaveVendorProduct( idx, out uint vendor, out uint product )) {
                if (vendor == eniInfo.Value.VendorId && product == eniInfo.Value.ProductCode) {
                    return idx;
                }
            }
        }

        // No exact match -> fallback to first slave (as in the Python script)
        return 1;
    }
}