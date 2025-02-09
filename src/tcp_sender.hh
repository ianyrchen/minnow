#pragma once

#include "byte_stream.hh"
#include "tcp_receiver_message.hh"
#include "tcp_sender_message.hh"

#include <functional>
#include <deque>

class TCPSender
{
public:
  /* Construct TCP sender with given default Retransmission Timeout and possible ISN */
  TCPSender( ByteStream&& input, Wrap32 isn, uint64_t initial_RTO_ms )
    : input_( std::move( input ) ), isn_( isn ), initial_RTO_ms_( initial_RTO_ms ),
      consecutive_retransmissions_( 0 ), received_ackno_( 0 ), receiver_window_size_( 1 ), 
      next_seqno_to_send_( isn ), current_RTO_( initial_RTO_ms ), time_since_last_send_( 0 ), 
      outstanding_segments_(), sent_fin_( false )
  {}

  /* Generate an empty TCPSenderMessage */
  TCPSenderMessage make_empty_message() const;

  /* Receive and process a TCPReceiverMessage from the peer's receiver */
  void receive( const TCPReceiverMessage& msg );

  /* Type of the `transmit` function that the push and tick methods can use to send messages */
  using TransmitFunction = std::function<void( const TCPSenderMessage& )>;

  /* Push bytes from the outbound stream */
  void push( const TransmitFunction& transmit );

  /* Time has passed by the given # of milliseconds since the last time the tick() method was called */
  void tick( uint64_t ms_since_last_tick, const TransmitFunction& transmit );

  // Accessors
  uint64_t sequence_numbers_in_flight() const;  // For testing: how many sequence numbers are outstanding?
  uint64_t consecutive_retransmissions() const; // For testing: how many consecutive retransmissions have happened?
  const Writer& writer() const { return input_.writer(); }
  const Reader& reader() const { return input_.reader(); }
  Writer& writer() { return input_.writer(); }

private:
  Reader& reader() { return input_.reader(); }

  ByteStream input_;
  Wrap32 isn_;
  uint64_t initial_RTO_ms_;
  uint64_t consecutive_retransmissions_;

  /*
  need to keep track of:
  - what the receiver wants next: ackno_, and a window size
  - outstanding segments: each has a: time left, length
  */

  struct Segment {
    TCPSenderMessage message_;
    uint64_t time_left_;
  };

  uint64_t received_ackno_;    
  uint16_t receiver_window_size_; 

  Wrap32 next_seqno_to_send_;  
  uint64_t current_RTO_;               
  uint64_t time_since_last_send_; 
  std::deque<Segment> outstanding_segments_; 
  bool sent_fin_;
};
