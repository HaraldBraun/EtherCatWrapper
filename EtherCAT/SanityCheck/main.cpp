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
      //return 1;
   }

   int slaveCount = ECW_GetSlaveCount();
   std::cout << "ECW_GetSlaveCount() = " << slaveCount << '\n';

   uint16_t val = 0;
   int rc = ECW_ReadEeprom(1, 0, &val);
   std::cout << "ECW_ReadEEprom(1, 0, &val) returned: " << rc
             << ", value = 0x" << std::hex << val << std::dec
             << " (" << val << ")\n";

   return 0;
}