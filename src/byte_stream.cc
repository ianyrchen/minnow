#include "byte_stream.hh"
#include <iostream>

using namespace std;

ByteStream::ByteStream( uint64_t capacity )
  : capacity_( capacity ), closed_ (false) {}

void Writer::push( string data )
{
    data.resize(min(data.size(), available_capacity()));
    buffer_ += data;
    bytes_pushed_ += data.size();
    /*
  if ( closed_ || capacity_ - ( bytes_pushed_ - bytes_popped_ ) == 0 ) {
    return;
  }
  uint16_t push_len = std::min( capacity_ - ( bytes_pushed_ - bytes_popped_ ), data.length() );
  bytes_pushed_ += push_len;
  for ( uint16_t i = 0; i < push_len; i++ ) {
    circular_[write_idx_] = data[i];
    write_idx_++;
    write_idx_ %= capacity_;
  }*/
}

void Writer::close()
{
  closed_ = true;
}

bool Writer::is_closed() const
{
  return closed_;
}

uint64_t Writer::available_capacity() const
{
  return capacity_ - buffer_.size();
}

uint64_t Writer::bytes_pushed() const
{
  return bytes_pushed_;
}

string_view Reader::peek() const
{
    return buffer_;
    /*
  static std::string next_bytes = "";

  // next_bytes persists through different peek calls so must ensure reset
  if ( next_bytes != "" )
    next_bytes = "";

  next_bytes += circular_[read_idx_];
  std::string_view out { next_bytes };
  return out;*/
}

void Reader::pop( uint64_t len )
{
    buffer_.erase(0, len);
    /*
  uint16_t pop_len = std::min( bytes_buffered(), len );
  bytes_popped_ += pop_len;
  for ( uint16_t i = 0; i < pop_len; i++ ) {
    read_idx_++;
    read_idx_ %= capacity_;
  }*/
}

bool Reader::is_finished() const
{
    return closed_ and buffer_.empty();
  //return closed_ && bytes_popped_ == bytes_pushed_;
}

uint64_t Reader::bytes_buffered() const
{
    return buffer_.size();
  //return bytes_pushed_ - bytes_popped_;
}

uint64_t Reader::bytes_popped() const
{
    return bytes_pushed_ - buffer_.size();
  //return bytes_popped_;
}
