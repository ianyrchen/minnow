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
        error_ = true;
        cerr << "Cannot write to closed ByteStream\n";
    }
    uint16_t push_len = std::min(capacity_ - byte_deque_.size(), data.length());
    bytes_pushed_ += push_len;
    //std::cout << "push_len " << push_len << std::endl;
    for (uint16_t i = 0; i < push_len; i++) {
        byte_deque_.push_back(data[i]);
    }
    
    //for (auto i : byte_deque_) std::cout << i;
    //std::cout << "\n";

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
    //std::cout << "\npeek\n";
    
    static std::string next_bytes = "";
    //cout << "next_bytes " << next_bytes << "\n";
    
    // next_bytes persists through different peek calls so must ensure reset
    if (next_bytes != "")
        next_bytes = "";
    
    uint16_t count = 0;
    for (char i : byte_deque_) {
        //char byte = byte_deque_.front();
        next_bytes.push_back(i);
        //byte_deque_.pop_front();
        //byte_deque_.push_back(byte);
        count++;
        // to pass speed
        if (count > PEEK_LEN) 
            break;
    }
    std::string_view out{ next_bytes };
    //std::cout << "\n" << out << "\n";
    //next_bytes = "";
    return out;

    //std::string s(byte_deque_.begin(), byte_deque_.end());
    //std::string_view out{ s };
    //return out;
}

void Reader::pop( uint64_t len )
{

    //std::cout << "\npopping len " << len << "\n";
    // for (auto i : byte_deque_) std::cout << i;
    //std::cout<<"\n";
    uint16_t pop_len = std::min(byte_deque_.size(), len);
    bytes_popped_ += pop_len;
    //std::cout << "\npop_len " << pop_len << "\n";
    for (uint16_t i = 0; i < pop_len; i++) {
        byte_deque_.pop_front();
    }

    //for (auto i : byte_deque_) std::cout << i;
    //std::cout<<"\n";
}

bool Reader::is_finished() const
{
    return closed_ && byte_deque_.empty();
}

uint64_t Reader::bytes_buffered() const
{
    return byte_deque_.size();
}

uint64_t Reader::bytes_popped() const
{
    return bytes_popped_;
}

