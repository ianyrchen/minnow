#include "tcp_receiver.hh"
#include "debug.hh"
#include <iostream>

#include <cstdint>
#include <cstdlib>

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message )
{
  const Writer& writer = reassembler_.writer();
  if ( message.RST ) {
    reader().set_error();
    rst_err_ = true;
  }
  if ( message.SYN )
    isn_ = Wrap32( message.seqno );

  uint64_t first_index = message.seqno.unwrap( *isn_, writer.bytes_pushed() );
  reassembler_.insert( first_index - !message.SYN, message.payload, message.FIN );
  // -!msg.SYN bc reassembler expects 0 as first data byte
}

TCPReceiverMessage TCPReceiver::send() const
{
  const Writer& writer = reassembler_.writer();
  TCPReceiverMessage msg = { {}, static_cast<uint16_t>( writer.available_capacity() ), rst_err_ };

  if ( isn_ != nullopt )
    msg.ackno = Wrap32::wrap( writer.bytes_pushed() + isn_.has_value() + writer.is_closed(), *isn_ );
  if ( writer.available_capacity() > UINT16_MAX )
    msg.window_size = UINT16_MAX;
  if ( writer.has_error() )
    msg.RST = true;

  return msg;
}
