// *****************************************************
//    Copyright 2023 Videonetics Technology Pvt Ltd
// *****************************************************

#include "raw_buffer.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <stdexcept>

constexpr int MEMORY_ALIGNMENT = 32;
constexpr int KB_600           = (1024 * 1024);

RawBuffer::RawBuffer(bool resize_always) : RawBuffer(nullptr, KB_600, resize_always) {}

RawBuffer::RawBuffer(const uint8_t* data_in, size_t valid_data_size, bool resize_always)
    : _resize_always(resize_always) {
  assign(data_in, valid_data_size);
}

void RawBuffer::resize(size_t new_size) {
  if (new_size > _buffer_capacity) {
    _buffer_capacity = new_size;
    _buffer_capacity = (_buffer_capacity % MEMORY_ALIGNMENT == 0)
                           ? _buffer_capacity
                           : ((_buffer_capacity / MEMORY_ALIGNMENT) * MEMORY_ALIGNMENT) + MEMORY_ALIGNMENT;
    // NOLINTNEXTLINE(hicpp-avoid-c-arrays,modernize-avoid-c-arrays,cppcoreguidelines-avoid-c-arrays)
    _buffer = std::make_unique<uint8_t[]>(_buffer_capacity);
  }
  _buffer_size = new_size;
}

RawBuffer::RawBuffer(const RawBuffer& other) { assign(other.data(), other.size()); }

auto RawBuffer::operator=(const RawBuffer& other) -> RawBuffer& {
  if (this == &other) {
    return *this;
  }
  assign(other.data(), other.size());
  return *this;
}

auto RawBuffer::data() const -> uint8_t* {
  if (_buffer_size == 0) {
    return nullptr;
  }
  return _buffer.get();
}

auto RawBuffer::data(size_t offset) const -> uint8_t* {
  if (_buffer_size == 0) {
    return nullptr;
  }
  if (offset >= size()) {
    throw std::runtime_error("reading outside of memory boundary");
  }
  return (_buffer.get() + offset);
}

auto RawBuffer::end() const -> uint8_t* {
  if (_buffer_size == 0) {
    return nullptr;
  }
  return (_buffer.get() + _buffer_size);
}

auto RawBuffer::size() const -> size_t { return _buffer_size; }

auto RawBuffer::capacity() const -> size_t { return _buffer_capacity; }

auto RawBuffer::assign(const uint8_t* data_in, size_t valid_data_size) -> void {
  resize(valid_data_size);
  if (data_in == nullptr) {
    return;
  }
  std::memcpy(_buffer.get(), data_in, valid_data_size);
}

auto RawBuffer::take(const uint8_t* data_in, size_t valid_data_size) -> void { assign(data_in, valid_data_size); }

auto RawBuffer::append(const uint8_t* data_in, size_t data_size) -> void {
  RawBuffer         temp_buff(data(), size());
  const std::size_t temp_size = size();
  resize(temp_size + data_size);
  std::memcpy(_buffer.get(), temp_buff.data(), temp_size);
  std::memcpy(_buffer.get() + temp_size, data_in, data_size);
}