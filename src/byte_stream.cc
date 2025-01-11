#include "byte_stream.hh"
#include <iostream>

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ), 
                                              byte_deque_(), 
                                              bytes_popped_(0), 
                                              bytes_pushed_(0),
                                              closed_(false) {}

void Writer::push( string data )
{
    if (closed_) {
        cerr << "Cannot write to closed ByteStream\n";
    }
    uint16_t push_len = std::min(capacity_ - byte_deque_.size(), data.length());
    bytes_pushed_ += push_len;
    for (uint16_t i = 0; i < push_len; i++) {
        byte_deque_.push_back(data[i]);
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
    return capacity_ - byte_deque_.size();
}

uint64_t Writer::bytes_pushed() const
{
    return bytes_pushed_;
}

string_view Reader::peek() const
{
    std::string next_bytes = "";
    for (auto i : byte_deque_) {
        //char byte = byte_deque_.front();
        next_bytes.push_back(i);
        //byte_deque_.pop_front();
        //byte_deque_.push_back(byte);
    }
    std::string_view out{ next_bytes };
    return out;
}

void Reader::pop( uint64_t len )
{
    uint16_t pop_len = std::min(byte_deque_.size(), len);
    bytes_popped_ += pop_len;
    for (uint16_t i = 0; i < pop_len; i++) {
        byte_deque_.pop_front();
    }
}

bool Reader::is_finished() const
{
    return closed_;
}

uint64_t Reader::bytes_buffered() const
{
    return bytes_pushed_;
}

uint64_t Reader::bytes_popped() const
{
    return bytes_popped_;
}

