#include "genie/dat/unit/TrainLocation.h"

namespace genie
{

namespace unit
{

void TrainLocation::serializeObject(void)
{
  GameVersion gv = getGameVersion();

  serialize<int16_t>(QueueTime);
  serialize<int16_t>(LocationID);
  serialize<uint8_t>(ButtonID);
  if (gv >= GV_C30 && gv <= GV_LatestDE2)
  {
    serialize<int32_t>(HotKeyID);
  }
}

}

}
