#include <iostream>

#include "arp_message.hh"
#include "debug.hh"
#include "ethernet_frame.hh"
#include "exception.hh"
#include "helpers.hh"
#include "network_interface.hh"

#include <vector>

using namespace std;

//! \param[in] ethernet_address Ethernet (what ARP calls "hardware") address of the interface
//! \param[in] ip_address IP (what ARP calls "protocol") address of the interface
NetworkInterface::NetworkInterface( string_view name,
                                    shared_ptr<OutputPort> port,
                                    const EthernetAddress& ethernet_address,
                                    const Address& ip_address )
  : name_( name )
  , port_( notnull( "OutputPort", move( port ) ) )
  , ethernet_address_( ethernet_address )
  , ip_address_( ip_address )
{
  cerr << "DEBUG: Network interface has Ethernet address " << to_string( ethernet_address_ ) << " and IP address "
       << ip_address.ip() << "\n";
}

//! \param[in] dgram the IPv4 datagram to be sent
//! \param[in] next_hop the IP address of the interface to send it to (typically a router or default gateway, but
//! may also be another host if directly connected to the same network as the destination) Note: the Address type
//! can be converted to a uint32_t (raw 32-bit IP address) by using the Address::ipv4_numeric() method.
void NetworkInterface::send_datagram( const InternetDatagram& dgram, const Address& next_hop )
{
    uint32_t raw_next_hop = next_hop.ipv4_numeric();
    if (ip_ethernet_map_.contains(raw_next_hop)) {
        /*
        send it right away. Create an Ethernet frame (with type = EthernetHeader::TYPE IPv4),
        set the payload to be the serialized datagram, and set the source and destination addresses
        */
        EthernetFrame frame;

        // make frame
        frame.payload = serialize( dgram );

        EthernetHeader header;
        header.dst = ip_ethernet_map_[raw_next_hop].first;
        header.src = ethernet_address_;
        header.type = 0x800; // IPv4

        frame.header = header;

        transmit(frame);
    }
    else {
        /*
        broadcast an ARP request for the next hop’s Ethernet address, and queue 
        the IP datagram so it can be sent after the ARP reply is received.
Except: You don’t want to flood the network with ARP requests. If the network
interface already sent an ARP request about the same IP address in the last
five seconds, don’t send a second request—just wait for a reply to the first one.
Again, queue the datagram until you learn the destination Ethernet address.
        */
        
        if (arp_request_timer_[raw_next_hop] == 0) {
            ARPMessage request;
            request.opcode = 1;
            request.sender_ethernet_address = ethernet_address_;
            request.sender_ip_address = ip_address_.ipv4_numeric();
            //request.target_ethernet_address = ETHERNET_BROADCAST;
            request.target_ip_address = next_hop.ipv4_numeric();

            EthernetFrame frame;
            frame.payload = serialize( request );

            EthernetHeader header;
            header.dst = ETHERNET_BROADCAST;
            header.src = ethernet_address_;
            header.type = 0x806; // ARP

            frame.header = header;
            
            transmit(frame);

            arp_request_timer_[raw_next_hop] = 5000; // 5 sec
        }
        not_sent_[raw_next_hop].push(dgram);
    }
}

//! \param[in] frame the incoming Ethernet frame
void NetworkInterface::recv_frame( EthernetFrame frame )
{
    if (frame.header.dst != ETHERNET_BROADCAST && frame.header.dst != ethernet_address_) {
        return;
    }
    
    //TYPE_IPv4 = 0x800, TYPE_ARP = 0x806; 
    if (frame.header.type == 0x800) {
        // IPv4
        InternetDatagram internet_datagram;
        if ( parse( internet_datagram, frame.payload ) ) {
            datagrams_received_.push(internet_datagram);
        }
    }
    else if (frame.header.type == 0x806) {
        // ARP
        ARPMessage arp_msg;
        if ( parse( arp_msg, frame.payload ) ) {
            /*
            remember the mapping between the sender’s IP address and Ethernet address for
            30 seconds. (Learn mappings from both requests and replies.) In addition, if it’s
            an ARP request asking for our IP address, send an appropriate ARP reply.
            */

            Address sender_addr = Address::from_ipv4_numeric(arp_msg.sender_ip_address);
            uint32_t raw_sender_addr = sender_addr.ipv4_numeric();
            ip_ethernet_map_[raw_sender_addr] = { arp_msg.sender_ethernet_address, 30000 };

            // request 
            if (arp_msg.opcode == 1) {
                ARPMessage reply;
                reply.opcode = 2;
                reply.sender_ethernet_address = ethernet_address_;
                reply.sender_ip_address = ip_address_.ipv4_numeric();
                reply.target_ethernet_address = arp_msg.sender_ethernet_address;
                reply.target_ip_address = arp_msg.sender_ip_address;
                
                EthernetFrame out_frame;
                out_frame.payload = serialize( reply );

                EthernetHeader header;
                header.dst = arp_msg.sender_ethernet_address;
                header.src = ethernet_address_;
                header.type = 0x806; // ARP

                out_frame.header = header;
                
                transmit(out_frame);
            }
            else {
                // reply, see if there's any queued stuff we need to send out
                while (!not_sent_[raw_sender_addr].empty()) {
                    InternetDatagram cur = not_sent_[raw_sender_addr].front();
                    not_sent_[raw_sender_addr].pop();
                    EthernetFrame out_frame;

                    // make frame
                    out_frame.payload = serialize( cur );

                    EthernetHeader header;
                    header.dst = ip_ethernet_map_[raw_sender_addr].first;
                    header.src = ethernet_address_;
                    header.type = 0x800; // IPv4

                    out_frame.header = header;

                    transmit(out_frame);
                }
            }
        }
    }
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void NetworkInterface::tick( const size_t ms_since_last_tick )
{
    vector<uint32_t> expired;
    for (auto i : ip_ethernet_map_) {
        if (i.second.second <= ms_since_last_tick) {
            expired.push_back(i.first);
        }
        else {
            i.second.second -= ms_since_last_tick;
        }
    }
    for (auto addr : expired) {
        ip_ethernet_map_.erase(addr);
    }

    for (auto i : arp_request_timer_) {
        if (i.second <= ms_since_last_tick) {
            i.second = 0;
        }
        else {
            i.second -= ms_since_last_tick;
        }
    }
}
