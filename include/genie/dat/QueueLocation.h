#ifndef GENIE_QUEUELOCATION_H
#define GENIE_QUEUELOCATION_H
#include "genie/file/ISerializable.h"

namespace genie
{

class QueueLocation : public ISerializable
{
public:
  QueueLocation() = default;
  QueueLocation(int16_t LocationID, int16_t QueueTime, uint8_t ButtonID, int32_t HotKeyID)
    : LocationID(LocationID), QueueTime(QueueTime), ButtonID(ButtonID), HotKeyID(HotKeyID)
  {
  }

  int16_t LocationID = -1;
  int16_t QueueTime = 0;
  uint8_t ButtonID = 0;
  int32_t HotKeyID = -1;
};

}

#endif //GENIE_QUEUELOCATION_H
