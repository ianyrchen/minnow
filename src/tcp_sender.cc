#include "tcp_sender.hh"
#include "debug.hh"
#include "tcp_config.hh"

using namespace std;

// This function is for testing only; don't add extra state to support it.
uint64_t TCPSender::sequence_numbers_in_flight() const
{
    uint64_t total = 0;
    for (auto segment : outstanding_segments_) {
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
    debug("in push");

    if (next_seqno_to_send_ == isn_) {
        TCPSenderMessage msg;
        msg.seqno = next_seqno_to_send_;
        msg.payload = "";  
        msg.SYN = true;    
        msg.FIN = false;   
        
        transmit(msg);
        outstanding_segments_.push_back({msg, current_RTO_});
        next_seqno_to_send_ = next_seqno_to_send_ + msg.sequence_length();
        return;
    }

    while (reader().bytes_buffered() > 0 && 
           outstanding_segments_.size() < receiver_window_size_) {

        uint16_t payload_size = std::min(uint16_t(TCPConfig::MAX_PAYLOAD_SIZE), receiver_window_size_);
        debug("payload sizes: {}, {}", uint16_t(TCPConfig::MAX_PAYLOAD_SIZE), receiver_window_size_);
        //debug( "Payload size: {}", payload_size );

        std::string_view payload = reader().peek();
        if (payload.size() > payload_size) {
            payload = payload.substr(0, payload_size);  // limit payload size
        }

        TCPSenderMessage msg;
        msg.seqno = next_seqno_to_send_;
        msg.payload = std::string(payload); 
        msg.SYN = (next_seqno_to_send_ == isn_);
        msg.FIN = reader().is_finished() && (reader().bytes_buffered() == 0);
        
        transmit(msg);
        outstanding_segments_.push_back({msg, current_RTO_});
        next_seqno_to_send_ = next_seqno_to_send_ + msg.sequence_length();
        
        reader().pop(payload.size());
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
    debug("received");
   if (msg.ackno.has_value()) {
        uint64_t new_ackno = msg.ackno->unwrap(isn_, received_ackno_);
        if (new_ackno > received_ackno_) {
            received_ackno_ = new_ackno;
            consecutive_retransmissions_ = 0;
            current_RTO_ = initial_RTO_ms_;
            time_since_last_send_ = 0;

            // Remove acknowledged segments from queue
            while (!outstanding_segments_.empty() &&
                   outstanding_segments_.front().message_.seqno.unwrap(isn_, received_ackno_) < received_ackno_) {
                outstanding_segments_.pop_front();
            }
        }
        receiver_window_size_ = msg.window_size;
        debug("now window size {}", receiver_window_size_);
   }
}

void TCPSender::tick( uint64_t ms_since_last_tick, const TransmitFunction& transmit )
{
    time_since_last_send_ += ms_since_last_tick;

    if (outstanding_segments_.empty()) {
        return;
    }

    // if enough time passred, transmit first outstanding segment
    if (time_since_last_send_ >= current_RTO_) {
        transmit(outstanding_segments_.front().message_);

        if (receiver_window_size_ > 0) {
            consecutive_retransmissions_++;
            current_RTO_ *= 2;
        }
        
        time_since_last_send_ = 0;
    }
}
