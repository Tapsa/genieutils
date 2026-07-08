#ifndef GENIE_TRAINLOCATION_H
#define GENIE_TRAINLOCATION_H
#include "../QueueLocation.h"

namespace genie
{

namespace unit
{

class TrainLocation : public QueueLocation
{
public:
  TrainLocation() = default;
  TrainLocation(int16_t UnitID, int16_t TrainTime, uint8_t ButtonID, int32_t HotKeyID)
    : QueueLocation(UnitID, TrainTime, ButtonID, HotKeyID)
  {
  }

private:
  void serializeObject(void) override;
};

}

}

#endif //GENIE_TRAINLOCATION_H
