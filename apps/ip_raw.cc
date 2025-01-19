#include "socket.hh"

using namespace std;

class RawSocket : public DatagramSocket
{
public:
  RawSocket() : DatagramSocket( AF_INET, SOCK_RAW, IPPROTO_RAW ) {}
};

auto zeroes( auto n )
{
  return string( n, 0 );
}

int main()
{
  // construct an Internet or user datagram here, and send using the RawSocket as in the Jan. 10 lecture
  RawSocket sock;

  string datagram;
  datagram += char( 0b0100'0101 ); // v4, and headerlength 5 words
  datagram += zeroes( 7 );

  datagram += char( 64 );  // TTL
  datagram += char( 5 );   // protocol
  datagram += zeroes( 6 ); // checksum + src address
  //10.144.0.8
  datagram += char( 10 );
  datagram += char( 144 );
  datagram += char( 0 );
  datagram += char( 8 );

  datagram += payload;

  sock.sendto( Address { "1" }, datagram );



  // send UDP
  datagram = "";
  datagram += char( 0b0100'0101 ); // v4, and headerlength 5 words
  datagram += zeroes( 7 );

  datagram += char( 64 );  // TTL
  datagram += char( 17 );   // protocol
  datagram += zeroes( 6 ); // checksum + src address
  //10.144.0.8
  datagram += char( 10 );
  datagram += char( 144 );
  datagram += char( 0 );
  datagram += char( 8 );

  // UDP
  string user_payload = "hi";
  datagram += char (0);
  datagram += char (user_payload.length() + 8);

  datagram += string( 2,0 );
  datagram += user_payload;

  sock.sendto( Address { "1" }, datagram );
  return 0;
}