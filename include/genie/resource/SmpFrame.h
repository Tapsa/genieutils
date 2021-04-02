/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2018 - 2021  Mikko "Tapsa" P

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

#ifndef GENIE_SMPFRAME_H
#define GENIE_SMPFRAME_H

#include "genie/file/ISerializable.h"
#include "genie/util/Logger.h"

#include <istream>
#include <vector>
#include <set>
#include <stdint.h>

#include "PalFile.h"

namespace genie
{

struct SmpFrameData
{
public:
  std::vector<uint16_t> pixel_indexes;
  std::vector<uint8_t> alpha_channel;
  std::vector<ColorXY> player_color_mask;
};

//------------------------------------------------------------------------------
/// This format is superseded by SMX.
/// Class for reading a frame of a SMP file. Once loaded the image can be
/// obtained as a pixel array. A pixel is stored as the index of a color
/// in a palette.
//
class SmpFrame : protected ISerializable
{
public:
  //----------------------------------------------------------------------------
  /// Constructor
  ///
  SmpFrame();

  //----------------------------------------------------------------------------
  /// Destructor
  //
  virtual ~SmpFrame();

  //----------------------------------------------------------------------------
  /// Loads frame data and creates an image. Frame data is located after all
  /// frame headers of the smp file.
  //
  void load(std::istream &istr, std::streampos offset);

  //----------------------------------------------------------------------------
  /// Get image's width.
  //
  inline uint32_t getWidth(void) const { return width_; }

  //----------------------------------------------------------------------------
  /// Get image's height.
  //
  inline uint32_t getHeight(void) const { return height_; }

  //----------------------------------------------------------------------------
  /// Get image's hotspot x.
  //
  inline int32_t getHotspotX(void) const { return hotspot_x_; }

  //----------------------------------------------------------------------------
  /// Get image's hotspot y.
  //
  inline int32_t getHotspotY(void) const { return hotspot_y_; }

  bool is32bit(void) const {return false;}

  //----------------------------------------------------------------------------
  /// Get the hotspot of the frame. The Hotspot is the isometric center of
  /// the object presented by this frame.

  uint32_t type;
  uint32_t diffuse_palette_xid;
  uint32_t diffuse_palette_num;

  SmpFrameData img_data;

private:
  static Logger &log;

  uint32_t layer_width_;
  uint32_t layer_height_;
  int32_t layer_hotspot_x;
  int32_t layer_hotspot_y;
  uint32_t layer_type_;
  uint32_t layer_outline_offsets_;
  uint32_t layer_data_offsets_;
  uint32_t layer_flags_;
  uint32_t width_;
  uint32_t height_;
  int32_t hotspot_x_ = 0;
  int32_t hotspot_y_ = 0;

  virtual void serializeObject(void);
};

typedef std::shared_ptr<SmpFrame> SmpFramePtr;

}

#endif // GENIE_SMPFRAME_H
