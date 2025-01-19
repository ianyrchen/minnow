#include "reassembler.hh"
#include "debug.hh"
#include <iostream>

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
    cout << "hi\n";

    Writer& writer = output_.writer();
    cout << writer.available_capacity() << " " << data << "\n";
    if (writer.available_capacity() >= data.length()) {
        if (first_index == bytes_pushed_to_output_) {
            cout << "hi\n";
            writer.push(data);
            bytes_pushed_to_output_ += data.length();
            // if overlap with stream_, delete overlapping keys of stream_
            for (uint64_t i = first_index; i < first_index + data.length(); i++) {
                stream_.erase(i); // delete key
            }
        }
        else {
            if (first_index >= bytes_pushed_to_output_ && first_index <= bytes_pushed_to_output_ + writer.available_capacity()) {
                for (uint64_t i = 0; i < data.length(); i++) {
                    stream_[first_index + i] = data[i]; // insert key
                } 
            }
        }
    }

    if (is_last_substring) {
        writer.close();
    }
}

// How many bytes are stored in the Reassembler itself?
// This function is for testing only; don't add extra state to support it.
uint64_t Reassembler::count_bytes_pending() const
{
  return stream_.size();
}
