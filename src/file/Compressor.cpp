/*
    genieutils - A library for reading and writing data files of genie
               engine games.
    Copyright (C) 2011 - 2013  Armin Preiml
    Copyright (C) 2026  Mikko "Tapsa" P

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

#include "genie/file/Compressor.h"

#include <zlib.h>
#include <vector>
#include <sstream>
#include <stdexcept>

namespace genie
{

// Helper: inflate (decompress) raw deflate data
static std::vector<char> inflateRaw(const std::vector<char> &compressed)
{
  if (compressed.empty())
    return {};

  z_stream strm{};
  strm.next_in = reinterpret_cast<Bytef *>(const_cast<char *>(compressed.data()));
  strm.avail_in = static_cast<uInt>(compressed.size());

  if (inflateInit2(&strm, -15) != Z_OK)
    throw std::runtime_error("inflateInit2 failed");

  std::vector<char> out;
  out.resize(compressed.size() * 8);
  size_t offset = 0;

  int ret = Z_OK;
  while (ret != Z_STREAM_END)
  {
    if (offset >= out.size())
      out.resize(out.size() * 2);

    strm.next_out = reinterpret_cast<Bytef *>(out.data() + offset);
    strm.avail_out = static_cast<uInt>(out.size() - offset);

    ret = inflate(&strm, Z_NO_FLUSH);
    if (ret == Z_STREAM_ERROR || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR)
    {
      inflateEnd(&strm);
      throw std::runtime_error("inflate failed");
    }

    offset += (out.size() - offset) - strm.avail_out;
  }

  inflateEnd(&strm);
  out.resize(offset);
  return out;
}

// Helper: deflate (compress) to raw deflate
static std::vector<char> deflateRaw(const std::vector<char> &uncompressed)
{
  if (uncompressed.empty())
    return {};

  z_stream strm{};
  if (deflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
                   -15, 9, Z_DEFAULT_STRATEGY) != Z_OK)
    throw std::runtime_error("deflateInit2 failed");

  strm.next_in = reinterpret_cast<Bytef *>(const_cast<char *>(uncompressed.data()));
  strm.avail_in = static_cast<uInt>(uncompressed.size());

  std::vector<char> out;
  out.resize(uncompressed.size());
  size_t offset = 0;

  int ret = Z_OK;
  while (ret != Z_STREAM_END)
  {
    if (offset >= out.size())
      out.resize(out.size() * 2);

    strm.next_out = reinterpret_cast<Bytef *>(out.data() + offset);
    strm.avail_out = static_cast<uInt>(out.size() - offset);

    ret = deflate(&strm, Z_FINISH);
    if (ret == Z_STREAM_ERROR)
    {
      deflateEnd(&strm);
      throw std::runtime_error("deflate failed");
    }

    offset += (out.size() - offset) - strm.avail_out;
  }

  deflateEnd(&strm);
  out.resize(offset);
  return out;
}

Compressor::Compressor()
{
}

//------------------------------------------------------------------------------
Compressor::Compressor(ISerializable *obj) :
  obj_(obj)
{
}

//------------------------------------------------------------------------------
Compressor::~Compressor()
{
}

//------------------------------------------------------------------------------
void Compressor::beginCompression(void)
{
  switch (obj_->getOperation())
  {
    case ISerializable::OP_READ:
      istream_ = obj_->getIStream();

      startDecompression();

      obj_->setIStream(*uncompressedIstream_);
      break;

    case ISerializable::OP_WRITE:
      obj_->setIStream(*istream_);

      startCompression();
      break;

    default:
      break;
  }
}

//------------------------------------------------------------------------------
void Compressor::endCompression(void)
{
  switch (obj_->getOperation())
  {
    case ISerializable::OP_READ:
      stopDecompression();
      break;

    case ISerializable::OP_WRITE:
      stopCompression();
      break;

    default:
      break;
  }
}

//------------------------------------------------------------------------------
void Compressor::decompress(std::istream &source, std::ostream &sink)
{
  // Read the whole compressed stream into a vector
  std::vector<char> compressed(
      (std::istreambuf_iterator<char>(source)),
      std::istreambuf_iterator<char>());

  if (compressed.empty())
    return;

  std::vector<char> uncompressed = inflateRaw(compressed);

  // Write the result to the sink
  sink.write(uncompressed.data(), static_cast<std::streamsize>(uncompressed.size()));
}

//------------------------------------------------------------------------------
void Compressor::startDecompression(void)
{
  try
  {
    // Read the whole compressed stream into a vector
    std::vector<char> compressed(
        (std::istreambuf_iterator<char>(*istream_)),
        std::istreambuf_iterator<char>());

    std::vector<char> uncompressed = inflateRaw(compressed);

    // Provide an istream over the uncompressed data
    auto *ss = new std::stringstream(
        std::string(uncompressed.begin(), uncompressed.end()));
    uncompressedIstream_.reset(ss);
  }
  catch (...)
  {
    uncompressedIstream_.reset();
    throw;
  }
}

//------------------------------------------------------------------------------
void Compressor::stopDecompression(void)
{
  istream_ = nullptr;
  uncompressedIstream_.reset();
}

//------------------------------------------------------------------------------
void Compressor::startCompression(void)
{
  // Just create an empty stringstream that the rest of the code will write into
  bufferedStream_ = std::make_shared<std::stringstream>();
  ostream_ = obj_->getOStream();
  obj_->setOStream(*bufferedStream_);
}

//------------------------------------------------------------------------------
void Compressor::stopCompression(void)
{
  try
  {
    // Get the data that was written into the buffered stream
    auto *ss = dynamic_cast<std::stringstream *>(bufferedStream_.get());
    if (!ss)
      throw std::runtime_error("unexpected stream type");

    std::string data = ss->str();
    std::vector<char> uncompressed(data.begin(), data.end());
    std::vector<char> compressed = deflateRaw(uncompressed);

    // Write the compressed data to the original output stream
    ostream_->write(compressed.data(), static_cast<std::streamsize>(compressed.size()));

    obj_->setOStream(*ostream_);
    ostream_ = nullptr;
    bufferedStream_.reset();
  }
  catch (...)
  {
    throw;
  }
}

}
