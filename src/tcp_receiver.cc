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
    fin_ = message.FIN;

    if (message.SYN) {
        isn_ = Wrap32(message.seqno);
    }    
    
    uint64_t first_index = message.seqno.unwrap(*isn_, writer.bytes_pushed() );
    debug( "writer bytes pushed: {}, reassembler count bytes pending: {}", writer.bytes_pushed(), reassembler_.count_bytes_pending());

    reassembler_.insert(first_index - !message.SYN, message.payload, message.FIN); // reassembler expects 0 as first data byte
    debug( "first_index: {}, payload: {}", first_index, message.payload);

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
    debug( "{}", writer.bytes_pushed());

    if (isn_ != nullopt)
        msg.ackno = Wrap32::wrap(writer.bytes_pushed() + 1 + fin_, *isn_);
    if (writer.available_capacity() > UINT16_MAX) 
        msg.window_size = UINT16_MAX;
    return msg;
}
