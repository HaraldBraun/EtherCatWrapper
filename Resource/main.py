import pysoem
from xml.etree import ElementTree as ET
import argparse
import sys
import os.path

class EthercatNode:
    def __init__(self, eni_file : str):
        self.eni_file = eni_file
        self.station_alias = 0x0012

    def initialize(self) -> int:
        print("=== PySOEM PoC (Alias Read/Write) ===")

        adapter_name = self.find_working_adapter()

        if adapter_name is not None:
            print(f"\nUsing adapter: {adapter_name}")

            self.master = pysoem.Master()

            self.master.open(adapter_name)

            if self.master.config_init() <= 0:
                print("❌ No slaves found")
                return 6001 # Slave not found

            print(f"✅ Found {len(self.master.slaves)} slave(s)")

            self.target = self.find_target_slave(self.master.slaves, self.eni_file)
            if self.target is not None:

                print("\n✅ Target slave:")
                print(f"  name={self.target.name}, vendor={self.target.man}, product={self.target.id}")

                # --- PREOP ---
                self.master.state = pysoem.PREOP_STATE
                self.master.write_state()
                self.master.state_check(pysoem.PREOP_STATE, 5000)

                print(f"State: {self.master.state}")
                return 0
            else:
                return 6003 # Eni file not correct or target slave not found according to it
        else:
            return 6002 # Adapter not found

    def __delete__(self, instance):
        self.master.close()

    def get_vendor_product_from_eni(self) -> int:
        """
        Extracts VendorId and ProductCode from ENI file.
        Returns: (vendor_id, product_code)
        """
        try:
            tree = ET.parse(self.eni_file)
            root = tree.getroot()

            # Find first slave (you only have 1 → simple)
            slave = root.find(".//Slave/Info")

            if slave is None:
                raise Exception("No Slave Info found in ENI")

            vendor_id = int(slave.findtext("VendorId"))
            product_code = int(slave.findtext("ProductCode"))

            return vendor_id, product_code
        except Exception as e:
            return None

    def find_working_adapter(self):
        adapters = pysoem.find_adapters()

        for nic in adapters:
            print(f"Trying adapter: {nic.name}")

            master = pysoem.Master()
            try:
                master.open(nic.name)

                if master.config_init() > 0:
                    print(f"✅ Working adapter found: {nic.name}")
                    master.close()
                    return nic.name

                master.close()

            except Exception as e:
                print(f"⚠ Failed on {nic.name}: {e}")

        return None

    def find_target_slave(self, slaves):
        vendor_id, target_product_code = self.get_vendor_product_from_eni()
        if vendor_id is not None:
            for s in slaves:
                if int(s.man) == vendor_id and int(s.id) == target_product_code:
                    return s
            print("⚠ Target not matched → fallback to first slave")
            return slaves[0]
        else:
            return None

    def write_alias(self, alias) -> int:
        # ============================================================
        # ✅ ALIAS WRITE
        # ============================================================
        print("\n--- EEPROM ALIAS WRITE ---")
        try:
            print(f"Writing new alias: {alias}")

            data = alias.to_bytes(2, 'little')
            self.target.eeprom_write(self.station_alias, data)

            print("✅ Write command sent")
            return 0

        except Exception as e:
            print("❌ EEPROM write failed:", e)
            return 6004

    def read_alias(self) -> str:
        print("\n--- EEPROM ALIAS READ ---")
        try:
            alias_raw = self.target.eeprom_read(self.station_alias, 1)
            current_alias = alias_raw[0]
            print(f"✅ Current alias: {current_alias}")
            return current_alias
        except Exception as e:
            print("❌ EEPROM read failed:", e)
            return None

def run(alias, eni_file):
    if os.path.exists(eni_file):
        ecat = EthercatNode(eni_file)
        init_err = ecat.initialize()

        if not init_err:
            ecat.write_alias(alias)
            if ecat.read_alias() != alias:
                sys.exit(6006) # Written value is not equal to the read value
        else:
            sys.exit(init_err)
    else:
        sys.exit(6007) # Eni file doesn't exist or not specified


if __name__ == "__main__": 
    parser = argparse.ArgumentParser()

    parser.add_argument("--alias", type=int, required=True)
    parser.add_argument("--eni_file", type=str, required=True)

    args = parser.parse_args()

    run(args.alias, args.eni_file)
else:
    print ("RUNNING TEST ENVIRONMENT, no inputs from the terminal are used")
    run(80, "C:\\Code_LV\\MKO\\Python Ethercat\\test_eni.xml")
    sys.exit(6010)