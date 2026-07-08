#ifndef GENIE_RESEARCHLOCATION_H
#define GENIE_RESEARCHLOCATION_H
#include "QueueLocation.h"

namespace genie
{

class ResearchLocation : public QueueLocation
{
public:
  ResearchLocation() = default;
  ResearchLocation(int16_t LocationID, int16_t ResearchTime, uint8_t ButtonID, int32_t HotKeyID)
    : QueueLocation(LocationID, ResearchTime, ButtonID, HotKeyID)
  {
  }

private:
  void serializeObject(void) override;
};

}

#endif //GENIE_RESEARCHLOCATION_H
