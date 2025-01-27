#include "reassembler.hh"

#include <numeric>

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  // keith's version from lab
  if ( is_last_substring ) {
    total_size_.emplace( first_index + data.size() );
  }

  buf_.resize( writer().available_capacity() );
  occupancy_.resize( writer().available_capacity() );

  auto first_unassembled = writer().bytes_pushed();
  auto first_unacceptable = first_unassembled + writer().available_capacity();

  auto left_wanted = max( first_unassembled, first_index );
  auto right_wanted = min( first_unacceptable, first_index + data.size() );
  auto left_in_data = left_wanted - first_index, right_in_data = right_wanted - first_index;
  auto left_in_buf = left_wanted - first_unassembled, right_in_buf = right_wanted - first_unassembled;

  if ( left_wanted < right_wanted ) {
    copy( data.begin() + left_in_data, data.begin() + right_in_data, buf_.begin() + left_in_buf );
    fill( occupancy_.begin() + left_in_buf, occupancy_.begin() + right_in_buf, true );
  }

  auto count = ranges::find( occupancy_, false ) - occupancy_.begin();
  output_.writer().push( buf_.substr( 0, count ) );
  buf_.erase( 0, count );
  occupancy_.erase( occupancy_.begin(), occupancy_.begin() + count );

  if ( total_size_.has_value() && total_size_.value() == writer().bytes_pushed() ) {
    output_.writer().close();
  }

  /*
    if ( is_last_substring ) {
      seen_finished_ = true;
      finished_byte_ = first_index + data.length();
    }

    Writer& writer = output_.writer();
    if ( first_index >= next_idx_needed_ + writer.available_capacity() ) {
      return;
    }

    // push the next available from within the data whenever possible
    while ( first_index <= next_idx_needed_ && next_idx_needed_ < first_index + data.length()
            && writer.available_capacity() > 0 ) {
      writer.push( string( 1, data[next_idx_needed_ - first_index] ) );
      // if overlap with stream_, delete overlapping keys of stream_
      stream_.erase( next_idx_needed_ ); // delete key
      next_idx_needed_++;
    }

    // push the immediate next keys in the reassembler if possible
    while ( stream_.find( next_idx_needed_ ) != stream_.end() && writer.available_capacity() > 0 ) {

      writer.push( string( 1, stream_[next_idx_needed_] ) );
      stream_.erase( next_idx_needed_ ); // delete key
      next_idx_needed_++;
    }

    // store everything else that we can
    uint64_t temp_index = first_index;
    while ( temp_index >= next_idx_needed_ && temp_index - first_index < data.length()
            && writer.available_capacity() > 0 && next_idx_needed_ + writer.available_capacity() > temp_index ) {

      stream_[temp_index] = data[temp_index - first_index];
      temp_index++;
    }

    if ( seen_finished_ && next_idx_needed_ == finished_byte_ ) {
      writer.close();
    }
    */
}

// How many bytes are stored in the Reassembler itself?
// This function is for testing only; don't add extra state to support it.
uint64_t Reassembler::count_bytes_pending() const
{
  // return stream_.size();
  return accumulate( occupancy_.begin(), occupancy_.end(), 0 );
}
