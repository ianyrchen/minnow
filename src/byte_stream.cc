#include "byte_stream.hh"
#include <iostream>

using namespace std;

ByteStream::ByteStream( uint64_t capacity )
  : capacity_( capacity ), circular_(capacity, '\0'), bytes_popped_(0),
    bytes_pushed_(0), closed_(false), read_idx_(0), write_idx_(0) {}

void Writer::push( string data )
{
  if ( closed_ || capacity_ - (bytes_pushed_ - bytes_popped_) == 0 ) {
    return;
    //error_ = true;
    //cerr << "Cannot write to closed ByteStream\n";
  }
  uint16_t push_len = std::min( capacity_ - (bytes_pushed_ - bytes_popped_), data.length() );
  bytes_pushed_ += push_len;
  for ( uint16_t i = 0; i < push_len; i++ ) {
    //byte_deque_.push_back( data[i] );
    circular_[write_idx_] = data[i];
    write_idx_++;
    write_idx_ %= capacity_;
  }
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
  return capacity_ - (bytes_pushed_ - bytes_popped_);
}

uint64_t Writer::bytes_pushed() const
{
  return bytes_pushed_;
}

string_view Reader::peek() const
{
  static std::string next_bytes = "";

  // next_bytes persists through different peek calls so must ensure reset
  if ( next_bytes != "" )
    next_bytes = "";

  next_bytes += circular_[read_idx_];
  //next_bytes += circular_[(read_idx_ + 1) % capacity_];
  //cout << next_bytes << '\n';
  std::string_view out { next_bytes };
  return out;
}

void Reader::pop( uint64_t len )
{
  uint16_t pop_len = std::min( bytes_buffered(), len );
  //cout << pop_len << "\n";
  bytes_popped_ += pop_len;
  for (uint16_t i = 0; i < pop_len; i++) {
    //byte_deque_.pop_front();
    read_idx_++;
    read_idx_ %= capacity_;
  }
}

bool Reader::is_finished() const
{
  return closed_ && bytes_popped_ == bytes_pushed_;
}

uint64_t Reader::bytes_buffered() const
{
    return bytes_pushed_ - bytes_popped_;
}

uint64_t Reader::bytes_popped() const
{
  return bytes_popped_;
}
