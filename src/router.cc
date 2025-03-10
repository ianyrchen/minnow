#include "router.hh"
#include "debug.hh"

#include <iostream>

using namespace std;

struct Route
{
  uint32_t route_prefix;
  uint8_t prefix_length;
  std::optional<Address> next_hop;
  size_t interface_num;
};

// route_prefix: The "up-to-32-bit" IPv4 address prefix to match the datagram's destination address against
// prefix_length: For this route to be applicable, how many high-order (most-significant) bits of
//    the route_prefix will need to match the corresponding bits of the datagram's destination address?
// next_hop: The IP address of the next hop. Will be empty if the network is directly attached to the router (in
//    which case, the next hop address should be the datagram's final destination).
// interface_num: The index of the interface to send the datagram out on.
void Router::add_route( const uint32_t route_prefix,
                        const uint8_t prefix_length,
                        const optional<Address> next_hop,
                        const size_t interface_num )
{
  cerr << "DEBUG: adding route " << Address::from_ipv4_numeric( route_prefix ).ip() << "/"
    << static_cast<int>( prefix_length ) << " => " << ( next_hop.has_value() ? next_hop->ip() : "(direct)" )
    << " on interface " << interface_num << "\n";
  Route r;
  r.route_prefix = route_prefix;
  r.prefix_length = prefix_length;
  r.next_hop = next_hop;
  r.interface_num = interface_num;
  routes_.push_back( r );
}

// Helper function to see if the datagram matches the given route
bool Router::match( const InternetDatagram& dgram, const Route& route )
{

  if ( route.prefix_length == 0 )
    return true;
  if ( route.prefix_length == 32 )
    return dgram.header.dst == route.route_prefix;

  if ( ( dgram.header.dst >> ( 32 - route.prefix_length ) ) << ( 32 - route.prefix_length ) == route.route_prefix )
    return true;
  return false;
}

// Go through all the interfaces, and route every incoming datagram to its proper outgoing interface.
void Router::route()
{

  for ( auto interface : interfaces_ ) {
    auto& datagrams_received = interface->datagrams_received();
    while ( !datagrams_received.empty() ) {

      InternetDatagram dgram = datagrams_received.front();
      datagrams_received.pop();

      // has sufficient ttl to continue onwards
      if ( dgram.header.ttl > 1 ) {

        Route best_route;
        int max_prefix_length = -1;

        for ( auto route : routes_ ) {
          if ( match( dgram, route ) ) {

            if ( max_prefix_length < route.prefix_length ) {
              max_prefix_length = route.prefix_length;
              best_route = route;
            }
          }
        }

        // did we match?
        if ( max_prefix_length > -1 ) {
          if ( !best_route.next_hop.has_value() ) {
            best_route.next_hop = Address::from_ipv4_numeric( dgram.header.dst );
          }

          dgram.header.ttl--;
          dgram.header.compute_checksum();

          Router::interface( best_route.interface_num )->send_datagram( dgram, *best_route.next_hop );
        }
      }
    }
  }
}
