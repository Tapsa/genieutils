#include "genie/dat/ResearchLocation.h"

namespace genie
{

void ResearchLocation::serializeObject(void)
{
  serialize<int16_t>(LocationID);
  serialize<int16_t>(QueueTime);
  serialize<uint8_t>(ButtonID);
  serialize<int32_t>(HotKeyID);
}

}
