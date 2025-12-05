// *****************************************************
//    Copyright 2023 Videonetics Technology Pvt Ltd
// *****************************************************

#include "raw_buffer.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <stdexcept>

constexpr int MEMORY_ALIGNMENT = 32;

RawBuffer::RawBuffer(size_t initial_size) : RawBuffer(nullptr, initial_size) {}

RawBuffer::RawBuffer(const uint8_t* data_in, size_t valid_data_size) { assign(data_in, valid_data_size); }

RawBuffer::~RawBuffer() { release(); }

RawBuffer::RawBuffer(const RawBuffer& other) { assign(other.data(), other.size()); }

RawBuffer& RawBuffer::operator=(const RawBuffer& other) {
  if (this != &other) {
    assign(other.data(), other.size());
  }
  return *this;
}

RawBuffer::RawBuffer(RawBuffer&& other) noexcept
    : buffer_(std::move(other.buffer_)), buffer_capacity_(other.buffer_capacity_), buffer_size_(other.buffer_size_),
      initial_capacity_(other.initial_capacity_), plus_minus_(other.plus_minus_) {
  other.buffer_capacity_  = 0;
  other.buffer_size_      = 0;
  other.initial_capacity_ = 0;
  other.plus_minus_       = 0;
}

RawBuffer& RawBuffer::operator=(RawBuffer&& other) noexcept {
  if (this != &other) {
    release(); // Free existing memory before stealing
    buffer_           = std::move(other.buffer_);
    buffer_capacity_  = other.buffer_capacity_;
    buffer_size_      = other.buffer_size_;
    initial_capacity_ = other.initial_capacity_;
    plus_minus_       = other.plus_minus_;

    other.buffer_capacity_  = 0;
    other.buffer_size_      = 0;
    other.initial_capacity_ = 0;
    other.plus_minus_       = 0;
  }

  return *this;
}

void RawBuffer::resize(size_t new_size) {
  if (new_size > 0) {
    bool resize_required = false;
    if (new_size >= initial_capacity_) {
      plus_minus_ += 2;
    }
    plus_minus_ -= 1;
    if (plus_minus_ < 0) {
      plus_minus_     = 0;
      resize_required = true;
    }
    if (new_size > buffer_capacity_) {
      resize_required = true;
    }

    auto aligned_size = (std::max(new_size, initial_capacity_) + MEMORY_ALIGNMENT - 1) & ~(MEMORY_ALIGNMENT - 1);
    if (aligned_size == buffer_capacity_) {
      resize_required = false;
    }

    if (resize_required) {

      if (initial_capacity_ == 0) {
        initial_capacity_ = aligned_size;
      }
      if (buffer_capacity_ > 0) {
        if (aligned_size > buffer_capacity_) {
          // std::cout << "+++ resizing up  : from: " << buffer_capacity_ << " to: " << aligned_size
          //           << std::endl;
        } else {
          // std::cout << "--- resizing down: from: " << buffer_capacity_ << " to: " << aligned_size << std::endl;
        }
      }

      release(); // remove old memory from auditor
      buffer_          = std::make_unique<uint8_t[]>(aligned_size);
      buffer_capacity_ = aligned_size;
      RawBufferMemoryAuditor::instance().AddToTotalMemory(buffer_capacity_);
    }
  }
  buffer_size_ = new_size;
}

void RawBuffer::release() {
  if (buffer_) {
    RawBufferMemoryAuditor::instance().RemoveFromTotalMemory(buffer_capacity_);
    // _buffer.reset();
    buffer_ = nullptr;
  }
  buffer_capacity_ = 0;
  buffer_size_     = 0;
}

void RawBuffer::assign(const uint8_t* data_in, size_t valid_data_size) {
  resize(valid_data_size);
  if (data_in) {
    std::memcpy(buffer_.get(), data_in, valid_data_size);
  }
}

uint8_t* RawBuffer::data() const { return (buffer_size_ > 0) ? buffer_.get() : nullptr; }

uint8_t* RawBuffer::end() const { return (buffer_size_ > 0) ? buffer_.get() + buffer_size_ : nullptr; }

uint8_t* RawBuffer::data(size_t offset) const {
  if (offset >= buffer_size_) {
    throw std::runtime_error("Offset out of bounds");
  }
  return buffer_.get() + offset;
}

void RawBuffer::append(const uint8_t* data_in, size_t data_size) {
  if (data_size == 0)
    return;

  const std::size_t          temp_size = buffer_size_;
  std::unique_ptr<uint8_t[]> temp_buff = std::make_unique<uint8_t[]>(temp_size);
  std::memcpy(temp_buff.get(), buffer_.get(), temp_size);
  resize(temp_size + data_size);
  std::memcpy(buffer_.get(), temp_buff.get(), temp_size);
  std::memcpy(buffer_.get() + temp_size, data_in, data_size);
}

void RawBuffer::take(const uint8_t* data_in, size_t valid_data_size) { assign(data_in, valid_data_size); }

void RawBuffer::clear() { buffer_size_ = 0; }

size_t RawBuffer::size() const { return buffer_size_; }
size_t RawBuffer::capacity() const { return buffer_capacity_; }

RawBufferMemoryAuditor& RawBufferMemoryAuditor::instance() {
  static RawBufferMemoryAuditor s_instance;
  return s_instance;
}

unsigned long long RawBufferMemoryAuditor::GetTotalMemory() const { return total_memory.load(); }

void RawBufferMemoryAuditor::AddToTotalMemory(unsigned long long total_size) {
  total_memory.fetch_add(total_size);
  // std::cout << "+++ RawBufferMemoryAuditor Final size " << GetTotalMemory() << std::endl;
}

void RawBufferMemoryAuditor::RemoveFromTotalMemory(unsigned long long total_size) {
  // if (total_memory.load() > 0) {
  total_memory.fetch_sub(total_size);
  // std::cout << "--- RawBufferMemoryAuditor Final size " << GetTotalMemory() << std::endl;
  // }
}