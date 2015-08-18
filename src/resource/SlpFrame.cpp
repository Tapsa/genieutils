/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  Armin Preiml
    Copyright (C) 2015  Mikko "Tapsa" P

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

#include "genie/resource/SlpFrame.h"

#include <iostream>
//Debug
#include <cassert>
#include <stdexcept>
#include <chrono>

#include "genie/resource/Color.h"

namespace genie
{

Logger& SlpFrame::log = Logger::getLogger("genie.SlpFrame");

//------------------------------------------------------------------------------
SlpFrame::SlpFrame()
{
}

//------------------------------------------------------------------------------
SlpFrame::~SlpFrame()
{
}

//------------------------------------------------------------------------------
void SlpFrame::setSlpFilePos(std::streampos pos)
{
  slp_file_pos_ = pos;
}

uint32_t SlpFrame::getWidth(void) const
{
  return width_;
}

uint32_t SlpFrame::getHeight(void) const
{
  return height_;
}

//----------------------------------------------------------------------------
const uint8_t* SlpFrame::getPixelIndexes(void) const
{
  return &img_data_.pixel_indexes[0];
}

SlpFrameData SlpFrame::getSlpFrameData(void) const
{
  return img_data_;
}

uint32_t SlpFrame::getPaletteOffset(void) const
{
  return palette_offset_;
}

uint32_t SlpFrame::getProperties(void) const
{
  return properties_;
}

bool SlpFrame::is32bit(void) const
{
  return (properties_ & 7) == 7;
}

void SlpFrame::serializeObject(void)
{
}

void SlpFrame::setLoadParams(std::istream &istr)
{
  setIStream(istr);
  setOperation(OP_READ);
}

void SlpFrame::setSaveParams(std::ostream &ostr, uint32_t &slp_offset_)
{
  setOStream(ostr);
  setOperation(OP_WRITE);
  std::chrono::time_point<std::chrono::system_clock> startTime = std::chrono::system_clock::now();

  assert(height_ < 4096);
  outline_table_offset_ = slp_offset_;
  cmd_table_offset_ = slp_offset_ + 4 * height_;
  slp_offset_ = cmd_table_offset_ + 4 * height_;

  // TODO: Build integers from image data.
  left_edges_.resize(height_);
  right_edges_.resize(height_);
  cmd_offsets_.resize(height_);
  commands_.resize(height_);
  for (uint32_t row = 0; row < height_; ++row)
  {
    cmd_offsets_[row] = slp_offset_;
    // Count left edge
    left_edges_[row] = 0;
    for (uint32_t col = 0; col < width_; ++col)
    {
      if (img_data_.bgra_channels[row * width_ + col] == 0x000000FF)
        ++left_edges_[row];
      else break;
    }
    // Fully transparent row
    if (left_edges_[row] == width_)
    {
      left_edges_[row] = 0x8000;
      continue;
    }
    // Read colors and count right edge
    uint32_t bgra, last_bgra = 0x000000FF;
    uint32_t pixel_set_size = 0;
    cnt_type count_type = CNT_DIFF;
    for (uint32_t col = left_edges_[row]; col < width_; ++col)
    {
      ++pixel_set_size;
      if (is32bit())
      {
        bgra = img_data_.bgra_channels[row * width_ + col];
        // Same color as last one
        if (last_bgra == bgra)
        {
          // Store set of different colors
          if (count_type == CNT_DIFF)
          {
            handleColors(CNT_DIFF, row, col, pixel_set_size - 2);
            pixel_set_size = 2;
          }
          count_type = CNT_SAME;
        }
        else
        {
          // Store set of same colors
          if (count_type == CNT_SAME)
          {
            handleColors(CNT_SAME, row, col, --pixel_set_size, bgra == 0x000000FF);
            pixel_set_size = 1;
          }
          count_type = CNT_DIFF;
        }
        last_bgra = bgra;
      }
    }
    // Handle last colors
    if (bgra == 0x000000FF)
    {
      right_edges_[row] = pixel_set_size;
    }
    else
    {
      right_edges_[row] = 0;
      handleColors(count_type, row, count_type != CNT_DIFF ? width_ : width_ + 1, pixel_set_size);
    }
    // End of line
    commands_[row].push_back(0x0F);
    slp_offset_ += commands_[row].size();
  }
#ifndef NDEBUG
  std::chrono::time_point<std::chrono::system_clock> endTime = std::chrono::system_clock::now();
  log.debug("Frame (%u bytes) encoding took [%u] milliseconds", slp_offset_ - outline_table_offset_, std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count());
#endif
}

//------------------------------------------------------------------------------
void SlpFrame::serializeHeader(void)
{
  serialize<uint32_t>(cmd_table_offset_);
  serialize<uint32_t>(outline_table_offset_);
  serialize<uint32_t>(palette_offset_);

  // 0x00 = use default palette
  // 0x08 = only 1 pcs in TC, seems to be useless leftover from AoE 1, mostly containing player colors.
  // 0x10 = tree SLPs 147 and 152 in RoR have two shadows, mask and black pixels. Has pure black shadow? No
  // 0x18 = use default palette, 0x08 uses outline? No
  serialize<uint32_t>(properties_);

  serialize<uint32_t>(width_);
  serialize<uint32_t>(height_);

  serialize<int32_t>(hotspot_x);
  serialize<int32_t>(hotspot_y);

/*#ifndef NDEBUG
  log.debug("Frame header [%u], [%u], [%u], [%u], [%u], [%u], [%d], [%d], ",
    cmd_table_offset_, outline_table_offset_, palette_offset_, properties_,
    width_, height_, hotspot_x, hotspot_y);
#endif*/
}

//------------------------------------------------------------------------------
void SlpFrame::load(std::istream &istr)
{
  setIStream(istr);
  std::chrono::time_point<std::chrono::system_clock> startTime = std::chrono::system_clock::now();

  if (is32bit())
    img_data_.bgra_channels.resize(width_ * height_);
  else
    img_data_.pixel_indexes.resize(width_ * height_);
  img_data_.alpha_channel.resize(width_ * height_, 0);

  uint16_t integrity = 0;
  istr.seekg(slp_file_pos_ + std::streampos(outline_table_offset_));
  readEdges(integrity);

  istr.seekg(slp_file_pos_ + std::streampos(cmd_table_offset_));
  serialize<uint32_t>(cmd_offsets_, height_);

  if (integrity != 0x8000) // At least one visible row.
  // Each row has it's commands, 0x0F signals the end of a rows commands.
  for (uint32_t row = 0; row < height_; ++row)
  {
    istr.seekg(slp_file_pos_ + std::streampos(cmd_offsets_[row]));
    assert(!istr.eof());
    // Transparent rows apparently read one byte anyway. NO THEY DO NOT! Ignore and use seekg()
    if (0x8000 == left_edges_[row] || 0x8000 == right_edges_[row]) // Remember signedness!
    {
      continue; // Pretend it does not exist.
    }
    uint32_t pix_pos = left_edges_[row]; //pos where to start putting pixels

    uint8_t data = 0;
    while (true)
    {
      data = read<uint8_t>();

      if (data == 0x0F) break;

      uint8_t cmd = data & 0x0F;
      uint8_t sub = data & 0xF0;

      uint32_t pix_cnt = 0;

      switch (cmd) //0x00
      {
        case 0x0: // Lesser block copy
        case 0x4:
        case 0x8:
        case 0xC:
          pix_cnt = (data & 0xFC) >> 2;
          if (is32bit())
            readPixelsToImage32(row, pix_pos, pix_cnt);
          else
            readPixelsToImage(row, pix_pos, pix_cnt);
          break;

        case 0x1: // lesser skip (making pixels transparent)
        case 0x5:
        case 0x9:
        case 0xD:
          pix_cnt = (data & 0xFC) >> 2;
          pix_pos += pix_cnt;
          break;

        case 0x2: // greater block copy
          pix_cnt = (sub << 4) + read<uint8_t>();
          if (is32bit())
            readPixelsToImage32(row, pix_pos, pix_cnt);
          else
            readPixelsToImage(row, pix_pos, pix_cnt);
          break;

        case 0x3: // greater skip
          pix_cnt = (sub << 4) + read<uint8_t>();
          pix_pos += pix_cnt;
          break;

        case 0x6: // copy and transform (player color)
          pix_cnt = getPixelCountFromData(data);
          if (is32bit())
            readPixelsToImage32(row, pix_pos, pix_cnt, true);
          else
            readPixelsToImage(row, pix_pos, pix_cnt, true);
          break;

        case 0x7: // Run of plain color
          pix_cnt = getPixelCountFromData(data);
          if (is32bit())
            setPixelsToColor32(row, pix_pos, pix_cnt);
          else
            setPixelsToColor(row, pix_pos, pix_cnt);
          break;

        case 0xA: // Transform block (player color)
          pix_cnt = getPixelCountFromData(data);
          if (is32bit())
            setPixelsToColor32(row, pix_pos, pix_cnt, true);
          else
            setPixelsToColor(row, pix_pos, pix_cnt, true);
          break;

        case 0xB: // Shadow pixels
          pix_cnt = getPixelCountFromData(data);
          setPixelsToShadow(row, pix_pos, pix_cnt);
          break;

        case 0xE: // extended commands.. TODO
          switch (data)
          {
            case 0x0E: //xflip?? skip?? TODO
            case 0x1E:
              log.error("Cmd [%X] not implemented", data);
              return;
              //row-= 1;
              break;

            case 0x2E:
            case 0x3E:
              log.error("Cmd [%X] not implemented", data);
              return;
              break;

            case 0x4E:
              setPixelsToOutline(row, pix_pos, 1);//, 242);
              break;
            case 0x6E:
              setPixelsToOutline(row, pix_pos, 1);//, 0);
              break;

            case 0x5E:
              pix_cnt = read<uint8_t>();
              setPixelsToOutline(row, pix_pos, pix_cnt);//, 242);
              break;
            case 0x7E:
              pix_cnt = read<uint8_t>();
              setPixelsToOutline(row, pix_pos, pix_cnt);//, 0);
              break;
            case 0x9E: // Apparently some kind of edge blending to background.
              pix_cnt = read<uint8_t>();
              if (is32bit())
              {
                readPixelsToImage32(row, pix_pos, pix_cnt);
                break;
              }
            default:
              log.error("Cmd [%X] not implemented", data);
              return;
          }
          break;
        default:
          log.error("Unknown cmd [%X]", data);
          std::cerr << "SlpFrame: Unknown cmd at " << std::hex << std::endl;
          return;
      }
    }
  }
#ifndef NDEBUG
  std::chrono::time_point<std::chrono::system_clock> endTime = std::chrono::system_clock::now();
  log.debug("Frame at [%u], decoding took [%u] milliseconds", outline_table_offset_, std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count());
#endif
}

//------------------------------------------------------------------------------
void SlpFrame::readEdges(uint16_t &integrity)
{
  left_edges_.resize(height_);
  right_edges_.resize(height_);

  for (uint32_t row = 0; row < height_; ++row)
  {
    serialize<uint16_t>(left_edges_[row]);
    serialize<uint16_t>(right_edges_[row]);
    integrity |= left_edges_[row];
  }
}

//------------------------------------------------------------------------------
void SlpFrame::readPixelsToImage(uint32_t row, uint32_t &col,
                                 uint32_t count, bool player_col)
{
  uint32_t to_pos = col + count;
  while (col < to_pos)
  {
    uint8_t color_index = read<uint8_t>();
    assert(row * width_ + col < img_data_.pixel_indexes.size());
    img_data_.pixel_indexes[row * width_ + col] = color_index;
    img_data_.alpha_channel[row * width_ + col] = 255;
    if (player_col)
    {
      SlpFrameData::PlayerColorElement pce = {col, row, color_index};
      img_data_.player_color_mask.push_back(pce);
    }
    ++col;
  }
}

//------------------------------------------------------------------------------
void SlpFrame::readPixelsToImage32(uint32_t row, uint32_t &col,
                                 uint32_t count, bool player_col)
{
  uint32_t to_pos = col + count;
  while (col < to_pos)
  {
    uint32_t bgra = read<uint32_t>();
    assert(row * width_ + col < img_data_.bgra_channels.size());
    img_data_.bgra_channels[row * width_ + col] = bgra;
    if (player_col)
    {
    }
    ++col;
  }
}

//------------------------------------------------------------------------------
void SlpFrame::setPixelsToColor(uint32_t row, uint32_t &col, uint32_t count,
                                bool player_col)
{
  uint8_t color_index = read<uint8_t>();
  uint32_t to_pos = col + count;
  while (col < to_pos)
  {
    assert(row * width_ + col < img_data_.pixel_indexes.size());
    img_data_.pixel_indexes[row * width_ + col] = color_index;
    img_data_.alpha_channel[row * width_ + col] = 255;
    if (player_col)
    {
      SlpFrameData::PlayerColorElement pce = {col, row, color_index};
      img_data_.player_color_mask.push_back(pce);
    }
    ++col;
  }
}

//------------------------------------------------------------------------------
void SlpFrame::setPixelsToColor32(uint32_t row, uint32_t &col, uint32_t count,
                                bool player_col)
{
  uint32_t bgra = read<uint32_t>();
  uint32_t to_pos = col + count;
  while (col < to_pos)
  {
    assert(row * width_ + col < img_data_.bgra_channels.size());
    img_data_.bgra_channels[row * width_ + col] = bgra;
    if (player_col)
    {
    }
    ++col;
  }
}

//------------------------------------------------------------------------------
void SlpFrame::setPixelsToShadow(uint32_t row, uint32_t &col, uint32_t count)
{
  uint32_t to_pos = col + count;
  while (col < to_pos)
  {
    SlpFrameData::XY xy = {col, row};
    img_data_.shadow_mask.push_back(xy);
    ++col;
  }
}

//------------------------------------------------------------------------------
void SlpFrame::setPixelsToOutline(uint32_t row, uint32_t &col, uint32_t count)
{
  uint32_t to_pos = col + count;
  while (col < to_pos)
  {
    SlpFrameData::XY xy = {col, row};
    img_data_.outline_mask.push_back(xy);
    ++col;
  }
}

//------------------------------------------------------------------------------
uint8_t SlpFrame::getPixelCountFromData(uint8_t data)
{
  uint8_t pix_cnt;

  data = (data & 0xF0) >> 4;

  if (data == 0)
    pix_cnt = read<uint8_t>();
  else
    pix_cnt = data;

  return pix_cnt;
}

//------------------------------------------------------------------------------
void SlpFrame::handleColors(cnt_type count_type, uint32_t row, uint32_t col, uint32_t count, bool transparent)
{
  if (count == 0) return;
  --col;
  switch (count_type)
  {
    case CNT_SAME:
      if (transparent) // Transparent pixel.
      {
        if (count > 0x3F) // Greater skip.
        {
          commands_[row].push_back(0x3 | (count & 0xF00) >> 4);
          commands_[row].push_back(count);
        }
        else // Lesser skip.
        {
          commands_[row].push_back(0x1 | count << 2);
        }
      }
      else
      {
        if (count > 0xFF)
        {
          log.error("Too long plain color");
        }
        else if (count > 0xF)
        {
          commands_[row].push_back(0x7);
          commands_[row].push_back(count);
          pushPixelsToBuffer32(row, col, 1);
        }
        else
        {
          commands_[row].push_back(0x7 | count << 4);
          pushPixelsToBuffer32(row, col, 1);
        }
      }
      break;
    case CNT_DIFF:
      if (count > 0x3F) // Greater copy.
      {
        commands_[row].push_back(0x2 | (count & 0xF00) >> 4);
        commands_[row].push_back(count);
        pushPixelsToBuffer32(row, col - count, count);
      }
      else // Lesser copy.
      {
        commands_[row].push_back(count << 2);
        pushPixelsToBuffer32(row, col - count, count);
      }
      break;
    case CNT_FEATHER:
      break;
    case CNT_PLAYER:
      break;
    case CNT_OUTLINE:
      break;
    case CNT_SHADOW:
      break;
  }
}

//------------------------------------------------------------------------------
void SlpFrame::pushPixelsToBuffer32(uint32_t row, uint32_t col, uint32_t count)
{
  for (uint32_t pix = col; pix < count + col; ++pix)
  {
    uint32_t bgra = img_data_.bgra_channels[row * width_ + pix];
    commands_[row].push_back(bgra);
    commands_[row].push_back(bgra >> 8);
    commands_[row].push_back(bgra >> 16);
    commands_[row].push_back(bgra >> 24);
  }
}

//------------------------------------------------------------------------------
void SlpFrame::save(std::ostream &ostr)
{
  setOStream(ostr);
  std::chrono::time_point<std::chrono::system_clock> startTime = std::chrono::system_clock::now();

  //Write edges
  for (uint32_t row = 0; row < height_; ++row)
  {
    serialize<uint16_t>(left_edges_[row]);
    serialize<uint16_t>(right_edges_[row]);
  }

  //Write cmd offsets
  serialize<uint32_t>(cmd_offsets_, height_);
  cmd_offsets_.clear();

  for (auto &commands: commands_)
    for (auto &col: commands)
      serialize<uint8_t>(col);

  commands_.clear();
#ifndef NDEBUG
  std::chrono::time_point<std::chrono::system_clock> endTime = std::chrono::system_clock::now();
  log.debug("SLP frame data saving took [%u] milliseconds", std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count());
#endif
}

}
