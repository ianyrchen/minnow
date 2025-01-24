#include "wrapping_integers.hh"
#include "debug.hh"
#include <iostream>

using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  return zero_point + n % (1ULL << 32);
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  uint64_t abs_seq_no;
  if (this->raw_value_ >= zero_point.raw_value_) {
    abs_seq_no = this->raw_value_ - zero_point.raw_value_;
  }
  else {
    abs_seq_no = (1ULL << 32) - zero_point.raw_value_ + this->raw_value_;
  }
  uint64_t rem = checkpoint % (1ULL << 32);
  uint64_t head = checkpoint - rem;
  uint64_t result = head + abs_seq_no;
  if (rem > abs_seq_no) {
    if (rem - abs_seq_no >= (1ULL << 31) && result < UINT64_MAX - (1ULL << 32)) {
        return head + abs_seq_no + (1ULL << 32);
    }
  }
  else {
    if (abs_seq_no - rem >= (1ULL << 31) && result >= (1ULL << 32)) {
        return head + abs_seq_no - (1ULL << 32);
    }
  }
  return result;
}
