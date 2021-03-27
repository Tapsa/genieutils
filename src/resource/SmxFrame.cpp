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

#include "genie/resource/SmxFrame.h"

#include <iostream>
#include <cassert>
#include <stdexcept>
#include <chrono>

#include "genie/resource/Color.h"

namespace genie
{

Logger& SmxFrame::log = Logger::getLogger("genie.SmxFrame");
extern const char* CNT_SETS;

//------------------------------------------------------------------------------
SmxFrame::SmxFrame()
{
}

//------------------------------------------------------------------------------
SmxFrame::~SmxFrame()
{
}

uint32_t SmxFrame::getWidth(void) const
{
  return main_layer_.width;
}

uint32_t SmxFrame::getHeight(void) const
{
  return main_layer_.height;
}

int16_t SmxFrame::getHotspotX(void) const
{
  return main_layer_.hotspot_x;
}

int16_t SmxFrame::getHotspotY(void) const
{
  return main_layer_.hotspot_y;
}

void SmxFrame::serializeObject(void)
{
}

//------------------------------------------------------------------------------
void SmxFrame::load(std::istream &istr)
{
  setIStream(istr);
  setOperation(OP_READ);

  serialize<uint8_t>(layer_flags);
  serialize<uint8_t>(palette_id);
  serialize<uint32_t>(original_frame_size);

  uint32_t cmd_data_size;
  if (layer_flags & 1)
  {
    serialize<uint16_t>(main_layer_.width);
    serialize<uint16_t>(main_layer_.height);
    serialize<int16_t>(main_layer_.hotspot_x);
    serialize<int16_t>(main_layer_.hotspot_y);
    serialize<uint32_t>(main_layer_.size);
    serialize<uint32_t>(main_layer_.original_size);
    std::vector<uint16_t> left_edges(main_layer_.height);
    std::vector<uint16_t> right_edges(main_layer_.height);
    for (uint16_t row = 0; row < main_layer_.height; ++row)
    {
      serialize<uint16_t>(left_edges[row]);
      serialize<uint16_t>(right_edges[row]);
    }
    std::vector<uint8_t> cmd_data;
    uint32_t pixel_data_size;
    serialize<uint32_t>(cmd_data_size);
    serialize<uint32_t>(pixel_data_size);
    serialize<uint8_t>(cmd_data, cmd_data_size);

    // Unpack the pixels
    if (pixel_data_size % 5 != 0)
    {
      log.error("Bad pixel data");
      return;
    }
    std::vector<uint16_t> pixel_data;
    size_t block_count = pixel_data_size / 5;
    if (layer_flags & 8)
    {
      pixel_data.reserve(block_count * 2);
      for (size_t block = 0; block < block_count; ++block)
      {
        uint32_t payload = read<uint32_t>();
        pixel_data.push_back(1023 & payload);
        pixel_data.push_back(1023 & payload >> 10);
        read<uint8_t>();
      }
    }
    else
    {
      pixel_data.reserve(block_count * 4);
      for (size_t block = 0; block < block_count; ++block)
      {
        uint16_t byte1 = read<uint8_t>();
        uint16_t byte2 = read<uint8_t>();
        uint16_t byte3 = read<uint8_t>();
        uint16_t byte4 = read<uint8_t>();
        uint16_t byte5 = read<uint8_t>();
        byte1 |= byte5 << 8 & 768;
        byte2 |= byte5 << 6 & 768;
        byte3 |= byte5 << 4 & 768;
        byte4 |= byte5 << 2 & 768;
        pixel_data.push_back(byte1);
        pixel_data.push_back(byte2);
        pixel_data.push_back(byte3);
        pixel_data.push_back(byte4);
      }
    }

    img_data.pixel_indexes.resize(main_layer_.width * main_layer_.height);
    img_data.alpha_channel.resize(main_layer_.width * main_layer_.height, 0);

    // Each row has its commands.
    for (uint32_t row = 0, cmd_off = 0, pix_off = 0; row < main_layer_.height; ++row)
    {
      if (0xFFFF == left_edges[row])
      {
        continue; // Skip transparent lines.
      }
      // Position where to start putting pixels.
      uint32_t pix_pos = left_edges[row];
      while (true)
      {
        uint8_t data = cmd_data[cmd_off++];
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
              uint32_t color_index = pixel_data[pix_off++];
              uint32_t save_off = row * main_layer_.width + pix_pos;
              assert(save_off < img_data.pixel_indexes.size());
              img_data.pixel_indexes[save_off] = color_index;
              img_data.alpha_channel[save_off] = 255;
              ++pix_pos;
            }
            break;
          }
          case 0x2:
          {
            uint32_t to_pos = pix_pos + pix_cnt;
            while (pix_pos < to_pos)
            {
              uint32_t color_index = pixel_data[pix_off++];
              uint32_t save_off = row * main_layer_.width + pix_pos;
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
  }

  if (layer_flags & 2)
  {
    serialize<uint16_t>(shadow_layer_.width);
    serialize<uint16_t>(shadow_layer_.height);
    serialize<int16_t>(shadow_layer_.hotspot_x);
    serialize<int16_t>(shadow_layer_.hotspot_y);
    serialize<uint32_t>(shadow_layer_.size);
    serialize<uint32_t>(shadow_layer_.original_size);
    std::vector<uint16_t> left_edges(shadow_layer_.height);
    std::vector<uint16_t> right_edges(shadow_layer_.height);
    for (uint16_t row = 0; row < shadow_layer_.height; ++row)
    {
      serialize<uint16_t>(left_edges[row]);
      serialize<uint16_t>(right_edges[row]);
    }
    std::vector<uint8_t> cmd_data;
    serialize<uint32_t>(cmd_data_size);
    serialize<uint8_t>(cmd_data, cmd_data_size);

    // Each row has its commands.
    for (uint32_t row = 0, cmd_off = 0; row < shadow_layer_.height; ++row)
    {
      if (0xFFFF == left_edges[row])
      {
        continue; // Skip transparent lines.
      }
      // Position where to start putting pixels.
      uint32_t pix_pos = left_edges[row];
      while (true)
      {
        uint8_t data = cmd_data[cmd_off++];
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
          case 0x2:
          {
            uint32_t to_pos = pix_pos + pix_cnt;
            while (pix_pos < to_pos)
            {
              img_data.shadow_mask.emplace_back(pix_pos, row);
              ++pix_pos;
            }
            break;
          }
        }
      }
    }
  }

  if (layer_flags & 4)
  {
    serialize<uint16_t>(outline_layer_.width);
    serialize<uint16_t>(outline_layer_.height);
    serialize<int16_t>(outline_layer_.hotspot_x);
    serialize<int16_t>(outline_layer_.hotspot_y);
    serialize<uint32_t>(outline_layer_.size);
    serialize<uint32_t>(outline_layer_.original_size);
    std::vector<uint16_t> left_edges(outline_layer_.height);
    std::vector<uint16_t> right_edges(outline_layer_.height);
    for (uint16_t row = 0; row < outline_layer_.height; ++row)
    {
      serialize<uint16_t>(left_edges[row]);
      serialize<uint16_t>(right_edges[row]);
    }
    std::vector<uint8_t> cmd_data;
    serialize<uint32_t>(cmd_data_size);
    serialize<uint8_t>(cmd_data, cmd_data_size);

    // Each row has its commands.
    for (uint32_t row = 0, cmd_off = 0; row < outline_layer_.height; ++row)
    {
      if (0xFFFF == left_edges[row])
      {
        continue; // Skip transparent lines.
      }
      // Position where to start putting pixels.
      uint32_t pix_pos = left_edges[row];
      while (true)
      {
        uint8_t data = cmd_data[cmd_off++];
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
          case 0x2:
          {
            uint32_t to_pos = pix_pos + pix_cnt;
            while (pix_pos < to_pos)
            {
              img_data.outline_pc_mask.emplace_back(pix_pos, row);
              ++pix_pos;
            }
            break;
          }
        }
      }
    }
  }
}

//------------------------------------------------------------------------------
void SmxFrame::save(std::ostream &ostr)
{
  setOStream(ostr);
}

}
