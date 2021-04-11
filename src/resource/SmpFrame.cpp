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

#include "genie/resource/SmpFrame.h"

#include <iostream>
//Debug
#include <cassert>
#include <stdexcept>
#include <chrono>

#include "genie/resource/Color.h"

namespace genie
{

Logger& SmpFrame::log = Logger::getLogger("genie.SmpFrame");
extern const char* CNT_SETS;

//------------------------------------------------------------------------------
SmpFrame::SmpFrame()
{
}

//------------------------------------------------------------------------------
SmpFrame::~SmpFrame()
{
}

void SmpFrame::serializeObject(void)
{
}

//------------------------------------------------------------------------------
size_t SmpFrame::load(std::istream &istr, std::streampos offset)
{
  setIStream(istr);
  setOperation(OP_READ);
  istr.seekg(offset);

  serialize<uint32_t>(width_);
  serialize<uint32_t>(height_);
  serialize<int32_t>(hotspot_x_);
  serialize<int32_t>(hotspot_y_);
  serialize<uint32_t>(type);
  serialize<uint32_t>(diffuse_palette_xid);
  serialize<uint32_t>(diffuse_palette_num);
  uint32_t num_layers = read<uint32_t>();

  // Immediately after there is layer info.
  if (num_layers > 0)
  {
    serialize<uint32_t>(layer_width_);
    serialize<uint32_t>(layer_height_);
    serialize<int32_t>(layer_hotspot_x);
    serialize<int32_t>(layer_hotspot_y);
    serialize<uint32_t>(layer_type_);
    serialize<uint32_t>(layer_outline_offsets_);
    serialize<uint32_t>(layer_data_offsets_);
    serialize<uint32_t>(layer_flags_);
  }
  else
  {
     log.error("No layers");
     return sizeof(SmpFrame);
  }

  img_data.pixel_indexes.resize(layer_width_ * layer_height_);
  img_data.alpha_channel.resize(layer_width_ * layer_height_, 0);

  istr.seekg(offset + std::streampos(layer_outline_offsets_));
  std::vector<uint16_t> left_edges(layer_height_);
  std::vector<uint16_t> right_edges(layer_height_);
  for (uint32_t row = 0; row < layer_height_; ++row)
  {
    serialize<uint16_t>(left_edges[row]);
    serialize<uint16_t>(right_edges[row]);
  }

  istr.seekg(offset + std::streampos(layer_data_offsets_));
  std::vector<uint32_t> cmd_offsets(layer_height_);
  serialize<uint32_t>(cmd_offsets, layer_height_);

  // Each row has its commands.
  for (uint32_t row = 0; row < layer_height_; ++row)
  {
    if (0xFFFF == left_edges[row] || 0 == cmd_offsets[row])
    {
      continue; // Skip transparent lines.
    }
    uint32_t pix_pos = left_edges[row]; //Position where to start putting pixels
    istr.seekg(offset + std::streampos(cmd_offsets[row]));
    assert(!istr.eof());

    while (true)
    {
      uint8_t data = read<uint8_t>();
      uint8_t cmd = data & 0x3;

      if (0x3 == cmd)
      {
        break;
      }

      uint8_t pix_cnt = 1 + (data >> 2);

      switch (cmd)
      {
        case 0x0: // Transparent skip
          pix_pos += pix_cnt;
          break;

        case 0x1: // Copy pixels
        {
          uint32_t to_pos = pix_pos + pix_cnt;
          while (pix_pos < to_pos)
          {
            uint32_t color_index = read<uint32_t>();
            uint32_t save_off = row * layer_width_ + pix_pos;
            assert(save_off < img_data.pixel_indexes.size());
            img_data.pixel_indexes[save_off] = color_index;
            img_data.alpha_channel[save_off] = 255;
            ++pix_pos;
          }
          break;
        }
        case 0x2: // Player color pixels
        {
          uint32_t to_pos = pix_pos + pix_cnt;
          while (pix_pos < to_pos)
          {
            uint32_t color_index = read<uint32_t>();
            uint32_t save_off = row * layer_width_ + pix_pos;
            assert(save_off < img_data.pixel_indexes.size());
            img_data.pixel_indexes[save_off] = color_index;
            img_data.alpha_channel[save_off] = 255;
            img_data.player_color_mask.emplace_back(pix_pos, row, color_index);
            ++pix_pos;
          }
          break;
        }
      }
    }
  }

  size_t pixel_memory = img_data.pixel_indexes.capacity() * sizeof(uint16_t);
  size_t alpha_memory = img_data.alpha_channel.capacity() * sizeof(uint8_t);
  size_t player_memory = img_data.player_color_mask.capacity() * sizeof(ColorXY16);
  return sizeof(SmpFrame) + pixel_memory + alpha_memory + player_memory;
}

}
