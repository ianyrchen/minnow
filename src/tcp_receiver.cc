#include "tcp_receiver.hh"
#include "debug.hh"
#include <iostream>

#include <cstdint>
#include <cstdlib>

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message )
{
    const Writer& writer = reassembler_.writer();
    rst_err_ = message.RST;
    if (message.RST) 
        reader().set_error();

    if (message.SYN) {
        isn_ = Wrap32(message.seqno);
        //ackno_ = Wrap32(message.seqno + 1);
    }    
    
    uint64_t first_index = message.seqno.unwrap(*isn_, writer.bytes_pushed() );
    //debug( "writer bytes pushed: {}, reassembler count bytes pending: {}", writer.bytes_pushed(), reassembler_.count_bytes_pending());
    
    reassembler_.insert(first_index - !message.SYN, message.payload, message.FIN); // reassembler expects 0 as first data byte
    //debug( "first_index: {}, payload: {}", first_index, message.payload);

    if (writer.has_error())
        rst_err_ = true;
    //void set_error() { error_ = true; };       // Signal that the stream suffered an error.
  //bool has_error() const { return error_; }; // Has the stream had an error?
}

TCPReceiverMessage TCPReceiver::send() const
{

/*
struct TCPReceiverMessage
{
  std::optional<Wrap32> ackno {};
  uint16_t window_size {};
  bool RST {};
};*/
    const Writer& writer = reassembler_.writer();
    TCPReceiverMessage msg = {{}, static_cast<uint16_t>(writer.available_capacity()), rst_err_};
    //debug( "{}", writer.bytes_pushed());

    if (isn_ != nullopt)
        msg.ackno = Wrap32::wrap(writer.bytes_pushed() + 1 + writer.is_closed(), *isn_);
    if (writer.available_capacity() > UINT16_MAX) 
        msg.window_size = UINT16_MAX;
    if (writer.has_error()) {
        msg.RST = true;
    }
    debug("{} {}", writer.has_error(), msg.RST);
    return msg;
}
