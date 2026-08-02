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
    /// Extrahiert VendorId und ProductCode des ersten Slaves aus einer ENI-Datei.
    /// </summary>
    /// <returns>EniSlaveInfo bei Erfolg, null falls die Datei nicht lesbar/gültig ist.</returns>
    public static EniSlaveInfo? ParseFirstSlave( string eniFilePath ) {
        try {
            var doc = XDocument.Load(eniFilePath);

            // Entspricht root.find(".//Slave/Info") aus dem Python-Vorbild
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
            // Datei nicht vorhanden, ungültiges XML, oder Werte nicht parsbar
            return null;
        }
    }

    /// <summary>
    /// Sucht unter den bereits gefundenen Slaves (aus EtherCatMaster) denjenigen,
    /// der zu Vendor/Product aus der ENI-Datei passt.
    /// Fallback: erster Slave, falls keiner exakt passt (analog zum Python-Vorbild).
    /// </summary>
    /// <returns>1-basierter Slave-Index, oder -1 falls gar keine Slaves vorhanden sind.</returns>
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

        // Kein exakter Treffer → Fallback auf ersten Slave (wie im Python-Skript)
        return 1;
    }
}