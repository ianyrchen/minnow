#include "tcp_sender.hh"
#include "debug.hh"
#include "tcp_config.hh"

using namespace std;

// This function is for testing only; don't add extra state to support it.
uint64_t TCPSender::sequence_numbers_in_flight() const
{
  uint64_t total = 0;
  for ( auto segment : outstanding_segments_ ) {
    total += segment.message_.sequence_length();
  }
  return total;
}

// This function is for testing only; don't add extra state to support it.
uint64_t TCPSender::consecutive_retransmissions() const
{
  return consecutive_retransmissions_;
}

void TCPSender::push( const TransmitFunction& transmit )
{
  uint16_t temp_receiver_window_size = !sent_syn_;
  if ( receiver_window_size_.has_value() ) {
    temp_receiver_window_size = *receiver_window_size_;
  }

  if ( next_seqno_to_send_ == isn_ ) {
    TCPSenderMessage msg;
    msg.seqno = next_seqno_to_send_;
    msg.payload = "";
    msg.SYN = true;
    msg.FIN = writer().is_closed();

    transmit( msg );
    outstanding_segments_.push_back( { msg, current_RTO_ } );
    next_seqno_to_send_ = next_seqno_to_send_ + msg.sequence_length();
    sent_fin_ = msg.FIN;
    sent_syn_ = msg.SYN;
    return;
  }

  while ( reader().bytes_buffered() > 0 && temp_receiver_window_size > 0 ) {
    uint16_t max_payload_size = std::min( uint16_t( TCPConfig::MAX_PAYLOAD_SIZE ), temp_receiver_window_size );
    std::string_view payload = reader().peek();
    max_payload_size = min( uint16_t( payload.size() ), max_payload_size );
    payload = payload.substr( 0, max_payload_size ); // limit payload size
    temp_receiver_window_size -= payload.size();

    TCPSenderMessage msg;
    msg.seqno = next_seqno_to_send_;
    msg.payload = std::string( payload );
    msg.SYN = ( next_seqno_to_send_ == isn_ );
    msg.FIN = !sent_fin_ && writer().is_closed() && ( reader().bytes_buffered() == payload.size() )
              && temp_receiver_window_size > 0;

    if ( next_seqno_to_send_.unwrap( isn_, received_ackno_ ) )
      transmit( msg );
    if ( !timer_on_ ) {
      timer_on_ = true;
      time_since_last_send_ = 0;
    }
    outstanding_segments_.push_back( { msg, current_RTO_ } );
    next_seqno_to_send_ = next_seqno_to_send_ + msg.sequence_length();

    reader().pop( payload.size() );
    sent_fin_ = msg.FIN;
  }

  // send fin flag msg
  if ( reader().is_finished() && temp_receiver_window_size > 0 && !sent_fin_ ) {
    TCPSenderMessage fin_msg = make_empty_message();
    fin_msg.FIN = true;
    transmit( fin_msg );
    outstanding_segments_.push_back( { fin_msg, current_RTO_ } );
    next_seqno_to_send_ = next_seqno_to_send_ + 1;
    sent_fin_ = true;
  }

  if ( receiver_window_size_.has_value() ) {
    *receiver_window_size_ = temp_receiver_window_size;
  }
}

TCPSenderMessage TCPSender::make_empty_message() const
{
  TCPSenderMessage msg;
  msg.seqno = next_seqno_to_send_;
  return msg;
}

void TCPSender::receive( const TCPReceiverMessage& msg )
{
  if ( msg.ackno.has_value() ) {
    uint64_t new_ackno = msg.ackno->unwrap( isn_, received_ackno_ );
    if ( new_ackno > received_ackno_ && new_ackno <= next_seqno_to_send_.unwrap( isn_, received_ackno_ ) ) {
      received_ackno_ = new_ackno;
      consecutive_retransmissions_ = 0;
      current_RTO_ = initial_RTO_ms_;
      time_since_last_send_ = 0;

      while ( !outstanding_segments_.empty()
              && outstanding_segments_.front().message_.seqno.unwrap( isn_, received_ackno_ ) < received_ackno_ ) {
        outstanding_segments_.pop_front();
      }
    }
    receiver_window_size_ = msg.window_size;
  }
}

void TCPSender::tick( uint64_t ms_since_last_tick, const TransmitFunction& transmit )
{
  time_since_last_send_ += ms_since_last_tick;
  if ( outstanding_segments_.empty() ) {
    return;
  }

  if ( time_since_last_send_ >= current_RTO_ ) {
    transmit( outstanding_segments_.front().message_ );

    if ( ( receiver_window_size_.has_value() && *receiver_window_size_ > 0 )
         || !receiver_window_size_.has_value() ) {
      consecutive_retransmissions_++;
      current_RTO_ *= 2;
    }

    time_since_last_send_ = 0;
  }
}
