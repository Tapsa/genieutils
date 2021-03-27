/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2021  Mikko "Tapsa" P

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GENIE_SMXFRAME_H
#define GENIE_SMXFRAME_H

#include "genie/file/ISerializable.h"
#include "genie/util/Logger.h"

#include <istream>
#include <vector>
#include <set>
#include <stdint.h>

#include "PalFile.h"

namespace genie
{

struct SmxFrameData
{
public:
  std::vector<uint16_t> pixel_indexes;
  std::vector<uint8_t> alpha_channel;
  std::vector<PlayerColorXY> player_color_mask;
  std::vector<XY> shadow_mask;
  std::vector<XY> outline_pc_mask;
};

struct SmxLayerInfo
{
  friend class SmxFrame;
public:
  uint16_t width;
  uint16_t height;
  int16_t hotspot_x;
  int16_t hotspot_y;
  uint32_t size;
  uint32_t original_size;
};

//------------------------------------------------------------------------------
/// Class for reading a frame of a SMX file. Once loaded the image can be
/// obtained as a pixel array. A pixel is stored as the index of a color
/// in a palette.
//
class SmxFrame : protected ISerializable
{
public:
  //----------------------------------------------------------------------------
  /// Constructor
  ///
  SmxFrame();

  //----------------------------------------------------------------------------
  /// Destructor
  //
  virtual ~SmxFrame();

  //----------------------------------------------------------------------------
  /// Loads header and frame data and creates an image.
  //
  void load(std::istream &istr);
  void save(std::ostream &ostr);

  //----------------------------------------------------------------------------
  /// Get image's width.
  //
  uint32_t getWidth(void) const;

  //----------------------------------------------------------------------------
  /// Get image's height.
  //
  uint32_t getHeight(void) const;

  //----------------------------------------------------------------------------
  /// Get image's hotspot x.
  //
  int16_t getHotspotX(void) const;

  //----------------------------------------------------------------------------
  /// Get image's hotspot y.
  //
  int16_t getHotspotY(void) const;

  bool is32bit(void) const {return false;}

  int16_t hotspot_x;
  int16_t hotspot_y;
  uint8_t layer_flags;
  uint8_t palette_id;
  union
  {
    uint32_t original_frame_size;
    uint16_t unique_frame_id;
  };

  SmxFrameData img_data;

private:
  static Logger &log;

  SmxLayerInfo main_layer_;
  SmxLayerInfo shadow_layer_;
  SmxLayerInfo outline_layer_;

  virtual void serializeObject(void) override;
};

typedef std::shared_ptr<SmxFrame> SmxFramePtr;

}

#endif // GENIE_SMXFRAME_H
