#include "reassembler.hh"
#include "debug.hh"
#include <iostream>

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{  
    //cout << endl;
    /*for (auto i : stream_) {
        cout << "stream_ contains: " << i.first << ", " << i.second << endl;
    }*/
    if (is_last_substring) {
        seen_finished_ = true;
        finished_byte_ = first_index + data.length();
    }

    Writer& writer = output_.writer();
    //cout << writer.available_capacity() << " " << data << "\n";


    if (first_index >= next_idx_needed_ + writer.available_capacity()) {
        return;
    }

    // push the next available from within the data whenever possible
    while (first_index <= next_idx_needed_ 
            && next_idx_needed_ < first_index + data.length()
            && writer.available_capacity() > 0) {
        //cout << "found idx " << next_idx_needed_ << ": ";
        //cout << data[next_idx_needed_ - first_index] << "\n";

        writer.push(string(1, data[next_idx_needed_ - first_index]));
        // if overlap with stream_, delete overlapping keys of stream_
        stream_.erase(next_idx_needed_); // delete key
        next_idx_needed_++;
    }
    /*for (auto i : stream_) {
        cout << "stream_ contains: " << i.first << ", " << i.second << endl;
    }*/
    //cout << "count_bytes_pending: " << count_bytes_pending() << endl;

    // push the next keys in the reassembler if possible
    uint64_t sz = stream_.size();
    while (stream_.find(next_idx_needed_) != stream_.end() && writer.available_capacity() > 0) {
        //cout << "found existing idx " << next_idx_needed_ << ": ";
        //cout << stream_[next_idx_needed_] << "\n";

        writer.push(string(1, stream_[next_idx_needed_]));
        stream_.erase(next_idx_needed_); // delete key
        next_idx_needed_++;
    }
    if (stream_.size() > sz) cerr << "stream_ size increased\n";
    //cout << "count_bytes_pending: " << count_bytes_pending() << endl;


    // store everything else that we can
    uint64_t temp_index = first_index;
    while (temp_index >= next_idx_needed_ 
            &&  temp_index - first_index < data.length()
            && writer.available_capacity() > 0
            && next_idx_needed_ + writer.available_capacity() > temp_index) {
        
        //cout << "stored idx " << temp_index << ": ";
        //cout << data[temp_index - first_index] << "\n";

        //writer.push(string(1, data[next_idx_needed_ - first_index]));
        stream_[temp_index] = data[temp_index - first_index];        
        temp_index++;
    }
    //cout << "count_bytes_pending: " << count_bytes_pending() << endl;

    //cout << "is_last_substring: " << is_last_substring << endl;
    //cout << "next_idx_needed: " << next_idx_needed_;
    //cout << "; finished_byte_: " << finished_byte_ << endl;
    if (seen_finished_ && next_idx_needed_ == finished_byte_) {
        writer.close();
    }

    /*
    for (auto i : stream_) {
        cout << "stream_ contains: " << i.first << ", " << i.second << endl;
    }*/
}

// How many bytes are stored in the Reassembler itself?
// This function is for testing only; don't add extra state to support it.
uint64_t Reassembler::count_bytes_pending() const
{
  return stream_.size();
}
