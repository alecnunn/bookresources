
//  Author: Jon Watte, for Game Programming Gems 5 by Charles River Media.

//  DISCLAIMER: This source code is provided as illustration of a concept 
//  and may contain bugs. Use it only after you fully understand all its 
//  behavior. It is provided without any warranty or guarantee, including 
//  but not limited to guarantees of merchantability or fitness for a 
//  particular purpose. You may only use this code if you yourself agree 
//  to accept full responsibility for your such use and and direct and 
//  indirect consequences thereof, and agree to indemn and hold harmless 
//  the author or the publisher for any claim arising out of such use.

//  Provided you have purchased the book Game Programming Gems 5, you may 
//  use this code, or derivatives thereof, in your own programs where you 
//  retain copyright, assuming you agree to the foregoing disclaimer and 
//  release of liability.

//  Usage:
//  1. Build this program using the included sock_port.h and 
//     sock_port.cpp files for support.
//  2. Run an introducer server on some publicly available 
//     host using "progname introduce".
//  3. Run at least one hosting peer on some machine behind 
//     a NAT gateway (or not) and give the hosted game a name 
//     using "progname host introducer.mydomain.com mygamename"
//  4. List all available hosting games from some other machine 
//     possibly behind a NAT gateway, using
//     "progname list introducer.mydomain.com"
//  5. Last, make sure the client and the host can exchange 
//     data directly after being introduced by the introducer 
//     by joining the client to the host:
//     "progname joine introducer.mydomain.com mygamename 1.2.3.4:5"
//     where 1.2.3.4:5 is the address of "mygamename" as shown 
//     by the "list" command.

//  Notes:
//
//  This code does not implement a reliable messages-over-UDP protocol. 
//  In a real, production environment, you want to either do that, or at 
//  least use a system of acknowledge packets with time-outs and re-sends 
//  for actions such as registering that you're hosting a game, or 
//  receiving a list of hosted games. You may also want to add 
//  authentication using login with name and password, as well as possible 
//  encryption. The ENet library is often recommended for purposes of 
//  putting reliable messages on top of UDP.
//
//  A more complete library dealing with match-making and introduction is 
//  based on the same ideas, but not the same code, is available at 
//  http://libintroduce.sourceforge.net/ -- you may wish to use that, 
//  once you've understood how this code works.
//
//  You may also find the discussion in the Networking and Multiplayer 
//  forums at http://www.gamedev.net/ to be illuminating. Feel free to 
//  register (it's free!) and ask questions there.


#include "sock_port.h"

#include <string>
#include <set>


enum {
  // Names used won't be longer than 32 characters
  MAX_NAME_LEN = 32,
  // This is the baked-in port I'm trying to use for 
  // consistency.
  GAME_PORT = 8960,
};

// Request is the header of all packets used on 
// the UDP port.
struct Request {
  enum What {
    IDLE,           // Used to avoid time-outs
    HOST,           // Host -> Introducer: I'm hosting
    LIST,           // Client -> Introducer: Who's hosting?
    LIST_RESPONSE,  // Introducer -> Client: Here's who
    JOIN,           // Client -> Introducer: Join w/ host
    PEER_JOIN,      // Introducer -> Host: Join this guy
    PAYLOAD,        // Sample payload data packet
  };
  unsigned char what; // enum What, but smaller
};
// struct Request is followed by some other data if 
// necessary for the request

// For Request::What::HOST
struct HostRequest {
  unsigned char what;
  char name[ MAX_NAME_LEN ];
};

// For Request::What::LIST sent back to client
struct ListResponse {
  unsigned char what;
  char name[ MAX_NAME_LEN ];
  sockaddr_in addr;
};

// For Request::What::JOIN
struct JoinRequest {
  unsigned char what;
  char name[ MAX_NAME_LEN ];
};

// For Request::What::PEER_JOIN
struct PeerJoinRequest {
  unsigned char what;
  sockaddr_in addr;
};

// For Request::What::PAYLOAD
struct PayloadRequest {
  unsigned char what;
  char message[ MAX_NAME_LEN ];
};

// Keep track of peers hosting
struct Peer {
  std::string name;
  sockaddr_in addr;
  bool operator==( Peer const & o ) const {
    return name == o.name;
  }
  bool operator<( Peer const & o ) const {
    return name < o.name;
  }
};
// A real introducer would time out a hoster who hasn't 
// sent any packets for a little while.
std::set< Peer > gHosting;

// We only ever use a single socket
SOCKET gSocket = BAD_SOCKET_FD;

// Remember who the introducer is, if I resolve it.
sockaddr_in gIntroAddr;

// Remember who the host is, if provided.
sockaddr_in gHostAddr;

// Remember whether the joiner is done or not.
bool joiner_done = false;

// Am I the introducer server?
bool is_introducer = false;


//  These loops could be merged, but haven't been for clarity.
void run_introducer();
void run_lister( char const * introducer );
void run_hoster( char const * introducer, char const * name );
void run_joiner( char const * hoster, char const * name, char const * ip );


// Look at what specific function the user requests, and 
// dispatch to the appropriate handler.
int main( int argc, char * argv[] ) {
  if( argc == 2 && !strcmp( argv[1], "introduce" ) ) {
    run_introducer();
  }
  if( argc == 3 && !strcmp( argv[1], "list" ) ) {
    run_lister( argv[2] );
  }
  if( argc == 4 && !strcmp( argv[1], "host" ) ) {
    run_hoster( argv[2], argv[3] );
  }
  if( argc == 5 && !strcmp( argv[1], "join" ) ) {
    run_joiner( argv[2], argv[3], argv[4] );
  }
  // print a usage message if not used correctly
  fprintf( stderr, "usage: intro 'introduce'\n" );
  fprintf( stderr, "       intro 'list' introducer\n" );
  fprintf( stderr, "       intro 'host' introducer name\n" );
  fprintf( stderr, "       intro 'join' introducer name ip\n" );
  fprintf( stderr, "  introducer is a host name\n" );
  fprintf( stderr, "  hoster is a dotted-ip with port\n" );
  fprintf( stderr, "  name is a unique user name\n" );
  return 1;
}


// Allocate the single, global socket we'll use in all roles.
void allocate_socket() {
  if( gSocket != BAD_SOCKET_FD ) {
    ::closesocket( gSocket );
  }
  else {
    INIT_SOCKET_LIBRARY();
  }

  // It's not likely that UDP ever changes its protocol number, 
  // but Linux headers stopped defining the constant.
  int udpProto = ::getprotobyname( "udp" )->p_proto;
  // Create our UDP socket.
  gSocket = ::socket( PF_INET, SOCK_DGRAM, udpProto );
  if( gSocket == BAD_SOCKET_FD ) {
    fprintf( stderr, "::socket() failed\n" );
    exit( 2 );
  }

  // Bind to my port on all local interfaces.
  // Because I want to run multiple instances on the same 
  // machine, I try a sequence of ports.
  bool ok = false;
  // Once I've bound to a port, I want to re-use that port 
  // if I re-allocate the socket, so remember which port 
  // was being used using a static variable (this means 
  // I can only open a single socket per process using this 
  // code).
  static int portUsed = 0;

  //  reserve the base port for the introducer
  int base_port = GAME_PORT+1;
  if( is_introducer ) {
    base_port = GAME_PORT;
  }

  for( int port = GAME_PORT; port < GAME_PORT+10; ++port ) {
    sockaddr_in addr;
    memset( &addr, 0, sizeof( addr ) );
    addr.sin_family = AF_INET;
    if( portUsed ) {
      //  use the old port if set
      port = portUsed;
    }
    addr.sin_port = htons( port );

    // bind the socket to a specific port
    int r = ::bind( gSocket, (sockaddr *)&addr, sizeof( addr ) );
    if( r < 0 ) {
      fprintf( stderr, "::bind(%d) failed\n", port );
      if( portUsed ) {
        break;  //  if I can't re-use the old port, bail out
      }
    }
    else {
      fprintf( stderr, "bound on port %d\n", port );
      ok = true;
      portUsed = port;
      break;
    }
  }

  if( !ok ) {
    fprintf( stderr, "Could not bind on any wanted port.\n" );
    exit( 2 );
  }
}


// Resolve a text string to a socket address to use for
// connecting.  Text addresses always resolve to publicly
// available IP addresses, so we can only use them for the
// introducer.
sockaddr_in resolve_text_addr( char const * name ) {
  sockaddr_in addr;
  memset( &addr, 0, sizeof( addr ) );
  addr.sin_family = AF_INET;
  addr.sin_port = htons( GAME_PORT );

  // ::gethostbyname() blocks until it's done; I wish 
  // there were a better API for DNS resolution.
  hostent * he = ::gethostbyname( name );
  if( !he ) {
    fprintf( stderr, "%s: host not found\n", name );
    exit( 2 );
  }

  // Take the first address only, even if there are more.
  addr.sin_addr = *(in_addr *)he->h_addr_list[0];
  return addr;
}


// Resolve a numeric address to a socket address to use for
// connecting.  I use these as short-hand for this testing
// application; a real application would present the user with a
// click-and-choose user interface.
// The string is of the form aa.bb.cc.dd:port
sockaddr_in resolve_numeric_addr( char const * name ) {
  sockaddr_in addr;
  memset( &addr, 0, sizeof( addr ) );

  // Pick out the five numbers.
  int a, b, c, d, p, m = int(0xffffff00), mp = int(0xffff0000);
  if( 5 != sscanf( name, "%d.%d.%d.%d:%d", 
        &a, &b, &c, &d, &p ) ) {
    fprintf( stderr, "%s: must be a.b.c.d:p format\n", name );
    exit( 2 );
  }

  // Check arguments for correct range.
  if( (a&m) || (b&m) || (c&m) || (d&m) || (p&mp) ) {
    fprintf( stderr, "%s: bad address value\n", name );
    exit( 2 );
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons( p );
  // construct the correct big-endian address
  SIN_ADDR_UINT( addr.sin_addr ) = 
      htonl( ((a<<24)|(b<<16)|(c<<8)|d) );
  return addr;
}


// Return a string form of an IP address (not thread safe).
char const * ip_addr_text( sockaddr_in const & sin ) {
  unsigned short port = ntohs( sin.sin_port );
  unsigned int addr = ntohl( SIN_ADDR_UINT( sin.sin_addr ) );
  static char ret[ 64 ];
  sprintf( ret, "%d.%d.%d.%d:%d", (addr>>24)&0xff, 
      (addr>>16)&0xff, (addr>>8)&0xff, addr&0xff,
      port );
  return ret;
}


//

// Wait for a socket for a short while (convenience)
bool wait_for_socket( SOCKET s ) {
  fd_set fds_r;
  FD_ZERO( &fds_r );
  FD_SET( s, &fds_r );
  timeval timeout;
  timeout.tv_sec = 2;
  timeout.tv_usec = 0;
  int r = ::select( int(s)+1, &fds_r, 0, 0, &timeout );
  return (r > 0) ? FD_ISSET( s, &fds_r ) != 0 : 0;
}


//  There's a flaw in WinSock where I'll need to re-open 
//  the socket if we get CONNRESET on a socket. This is 
//  because it wedges the socket when it receives an 
//  ICMP for port-not-reachable, which can happen during 
//  NAT introduction negotiation.
bool maybe_reallocate_socket( int r ) {
  if( r < 0 ) {
    if( SOCKET_NEED_REOPEN( SOCKET_ERRNO ) ) {
      fprintf( stderr, "Re-allocating socket because of WinSock.\n" );
      allocate_socket();
    }
    return true;
  }
  return false;
}


// When the introducer gets a HOST packet, handle it.
void introducer_host( char * data, int len, sockaddr_in & sin ) {
  // check the packet length
  if( len != sizeof( HostRequest ) ) {
    fprintf( stderr, "Bad HOST length %d.\n", len );
    return;
  }

  // extract the packet and make sure it's terminated
  HostRequest & hr = *(HostRequest *)data;
  hr.name[ MAX_NAME_LEN-1 ] = 0;
  if( !hr.name[0] ) {
    fprintf( stderr, "Received attempt to HOST empty name.\n" );
    return;
  }

  // Record the fact that there's a new guy hosting (in the 
  // real world, we'd also add a timeout-at time, and do some 
  // login name/password checking).
  fprintf( stderr, "Received HOST from %s for %s.\n",
      ip_addr_text( sin ), hr.name );
  Peer p;
  p.name = hr.name;
  p.addr = sin;
  gHosting.insert( p );
}


// When the introducer gets a LIST packet, handle it.
void introducer_list( char * data, int len, sockaddr_in & sin ) {
  // check the packet length
  if( len != sizeof( Request ) ) {
    fprintf( stderr, "Bad LIST length %d.\n", len );
    return;
  }
  fprintf( stderr, "Sending host list to %s.\n",
      ip_addr_text( sin ) );

  // I just dump all available hosts back -- no limiting, 
  // no matching, and above all, no re-sending of lost packets.
  ListResponse lr;
  lr.what = Request::LIST_RESPONSE;
  for( std::set< Peer >::iterator ptr = gHosting.begin();
      ptr != gHosting.end(); ++ptr ) {
    // Construct a ListResponse packet
    // The string is guaranteed terminated correctly.
    strcpy( lr.name, (*ptr).name.c_str() );
    // This puts an address as data, but that's OK, as the 
    // address is publicly visible (comes from ::recvfrom()).
    lr.addr = (*ptr).addr;
    // Send the packet, ignoring errors (packet may be dropped 
    // anywhere, including in the network layer -- too bad).
    ::sendto( gSocket, (char const *)&lr, sizeof( lr ), 0, 
        (sockaddr *)&sin, sizeof( sin ) );
  }

  // Send a final, empty host name to terminate the list.
  lr.name[0] = 0;
  ::sendto( gSocket, (char const *)&lr, sizeof( lr ), 0, 
      (sockaddr *)&sin, sizeof( sin ) );
}


// When the introducer gets a JOIN packet, handle it.
void introducer_join( char * data, int len, sockaddr_in & sin ) {
  // Make sure the size is right
  if( len != sizeof( JoinRequest ) ) {
    fprintf( stderr, "Bad JOIN length %d.\n", len );
    return;
  }

  // Find out who is hosting (to avoid bouncing packets on 
  // us to unrelated third parties).
  JoinRequest & jr = *(JoinRequest *)data;
  Peer p;
  p.name = jr.name;
  std::set< Peer >::iterator ptr = gHosting.find( p );
  if( ptr == gHosting.end() ) {
    fprintf( stderr, "JOIN reqeust for unknown name '%s'\n", 
       jr.name );
    return;
  }
  fprintf( stderr, "Received JOIN from %s to %s\n",
      ip_addr_text( sin ), jr.name );

  // Formulate request to hosting peer to introduce.
  PeerJoinRequest pjr;
  pjr.what = Request::PEER_JOIN;
  // This puts an IP address as data, but that's OK, because 
  // the IP address is the publicly visible address.
  pjr.addr = sin;
  // Send without error checking (channel is unreliable anyway)
  ::sendto( gSocket, (char const *)&pjr, sizeof( pjr ), 0, 
      (sockaddr *)&(*ptr).addr, sizeof( sockaddr_in ) );
  // I'm assuming the joining peer has already tried to send 
  // data to the hosting peer, so only the hosting peer needs 
  // to be told about the joinee.
}


// Run the introducer service on this instance of the 
// application.
void run_introducer() {
  // I'm the introducer
  is_introducer = true;
  // Prevent run-aways.
  int badCnt = 0;
  // I'll need a socket.
  allocate_socket();

  // Now, I'll just do my thing.
  fprintf( stderr, "Introducer waiting for action.\n" );
  while( true ) {
    // Get a packet.
    struct sockaddr_in sin;
    socklen_t slen = sizeof( sin );
    char data[ 512 ];
    int r = ::recvfrom( gSocket, data, 512, 0, (sockaddr *)&sin,
        &slen );
    if( r < 1 ) {
      // bad packet, ignore
      fprintf( stderr, "Bad packet or error; length %d.\n", r );
      if( (r < 0) && (++badCnt > 10) ) {
        fprintf( stderr, "Too many sequential errors.\n" );
        exit( 2 );
      }
      continue;
    }
    badCnt = 0;

    // Do what I need with it. I like the clarity of an explicit switch().
    Request & req = *(Request *)data;
    switch( req.what ) {
      case Request::HOST:
        introducer_host( data, r, sin );
        break;
      case Request::LIST:
        introducer_list( data, r, sin );
        break;
      case Request::JOIN:
        introducer_join( data, r, sin );
        break;
      default:
          fprintf( stderr, "Ignoring unknown what code: %d\n",
              req.what );
    }
  }
}



// When lister receives a response, deal with it.
void lister_list_response( char * data, int len, sockaddr_in & sin )
{
  // Make sure the size is right.
  if( len != sizeof( ListResponse ) ) {
    fprintf( stderr, "Bad LIST_RESPONSE length %d\n", len );
    return;
  }
  // Make sure the name is terminated.
  ListResponse & lr = *(ListResponse *)data;
  lr.name[ MAX_NAME_LEN-1 ] = 0;
  if( !lr.name[0] ) {
    //  I'm done!
    fprintf( stderr, "That was all the hosts for now.\n" );
    exit( 0 );
  }
  // Tell the user about this particular response.
  fprintf( stdout, "Found: %s %s\n", ip_addr_text( lr.addr ), lr.name );
}


// Run as a client, asing an introducer for current hosting 
// peers.
void run_lister( char const * introducer )
{
  // Prevent run-aways.
  int badCnt = 0;
  // I'll need a socket.
  allocate_socket();
  // I'll need an address to talk to
  gIntroAddr = resolve_text_addr( introducer );

  // Ask for available games
  Request lr;
  lr.what = Request::LIST;
  fprintf( stderr, "Asking introducer %s for hosts.\n", 
      ip_addr_text( gIntroAddr ) );
  ::sendto( gSocket, (char const *)&lr, sizeof( lr ), 0, 
      (sockaddr *)&gIntroAddr, sizeof( gIntroAddr ) );

  // Now, I'll just sit waiting for responses
  while( true ) {
    // Get a packet.
    struct sockaddr_in sin;
    socklen_t slen = sizeof( sin );
    char data[ 512 ];
    int r = ::recvfrom( gSocket, data, 512, 0, (sockaddr *)&sin,
        &slen );
    if( r < 1 ) {
      // bad packet, ignore
      fprintf( stderr, "Bad packet or error; length %d.\n", r );
      if( (r < 0) && (++badCnt > 10) ) {
        fprintf( stderr, "Too many sequential errors.\n" );
        exit( 2 );
      }
      continue;
    }
    badCnt = 0;

    // Do what I need with it. I like the clarity of an explicit switch().
    Request & req = *(Request *)data;
    switch( req.what ) {
      case Request::LIST_RESPONSE:
        lister_list_response( data, r, sin );
        break;
      default:
          fprintf( stderr, "Ignoring unknown what code: %d\n",
              req.what );
    }
  }
}


//  When hoster receives PEER_JOIN, do something about it.
void hoster_peer_join( char * data, int len, sockaddr_in & sin ) {
  //  Make sure the size is right
  if( len != sizeof( PeerJoinRequest ) ) {
    fprintf( stderr, "Bad length for PEER_JOIN %d.\n", len );
    return;
  }

  //  I really should only accept PeerJoinRequest 
  //  messages if they come from my introducer host.
  if( gIntroAddr.sin_port != sin.sin_port ||
      (SIN_ADDR_UINT( gIntroAddr.sin_addr ) != 
       SIN_ADDR_UINT( sin.sin_addr )) ) {
    fprintf( stderr, "Received PEER_JOIN from %s.\n", 
        ip_addr_text( sin ) );
    return;
  }
  PeerJoinRequest & pjr = *(PeerJoinRequest *)data;

  //  Send a message (any message will do) to make sure our 
  //  NAT has a state table record for the peer.
  PayloadRequest pr;
  pr.what = Request::PAYLOAD;
  strcpy( pr.message, "hello" );
  //  Send without error checking; UDP is lossy anyway.
  ::sendto( gSocket, (char const *)&pr, sizeof( pr ), 0, 
      (sockaddr *)&pjr.addr, sizeof( pjr.addr ) );
}


//  When hoster receives PAYLOAD, do something about it.
void hoster_payload( char * data, int len, sockaddr_in & sin ) {
  PayloadRequest & pr = *(PayloadRequest *)data;
  pr.message[ MAX_NAME_LEN-1 ] = 0;
  fprintf( stdout, "Payload: %s (from %s)\n", pr.message, 
      ip_addr_text( sin ) );
  if( !strcmp( pr.message, "bye" ) ) {
    // do nothing, to terminate the loop
  }
  else {
    // echo back the same data
    ::sendto( gSocket, (char const *)&pr, sizeof( pr ), 0, 
        (sockaddr *)&sin, sizeof( sin ) );
  }
}


//  Run the main loop for the peer that is hosting. In this case, 
//  "hosting" means just sending a single payload packet containing 
//  the text "hello" to all joiners, and receiving one containing 
//  the text "bye".
void run_hoster( char const * introducer, char const * name )
{
  //  Need a socket
  allocate_socket();
  //  Need an introducer
  gIntroAddr = resolve_text_addr( introducer );

  //  Form a registration packet
  HostRequest hr;
  hr.what = Request::HOST;
  strncpy( hr.name, name, MAX_NAME_LEN );
  hr.name[ MAX_NAME_LEN-1 ] = 0;
  //  Send it (no error checking -- might get lost)
  fprintf( stderr, "Registering %s with introducer %s\n", 
      name, ip_addr_text( gIntroAddr ) );
  ::sendto( gSocket, (char const *)&hr, sizeof( hr ), 0, 
    (sockaddr *)&gIntroAddr, sizeof( gIntroAddr ) );

  //  Now, wait for joiners and payloads
  while( true ) {
    sockaddr_in sin;
    socklen_t slen = sizeof( sin );
    char data[ 512 ];
    //  Wait for an incoming packet
    int r = ::recvfrom( gSocket, data, 512, 0, 
        (sockaddr *)&sin, &slen );
    if( maybe_reallocate_socket( r ) ) {
      continue;
    }

    // Dispatch based on the "what" code in the packet.
    Request & req = *(Request *)data;
    switch( req.what ) {
      case Request::PEER_JOIN:
        hoster_peer_join( data, r, sin );
        break;
      case Request::PAYLOAD:
        hoster_payload( data, r, sin );
        break;
      default:
        fprintf( stderr, "Unknown what code: %d\n", req.what );
    }
  }
}


void joiner_payload( char * data, int len, sockaddr_in & sin ) {
  //  Is the size right?
  if( len != sizeof( PayloadRequest ) ) {
    fprintf( stderr, "Received bad PAYLOAD len %d\n", len );
    return;
  }

  //  If I receive payload from someone other than the host, 
  //  it's likely a forgery.
  if( sin.sin_port != gHostAddr.sin_port ||
      (SIN_ADDR_UINT( sin.sin_addr ) != 
       SIN_ADDR_UINT( gHostAddr.sin_addr )) ) {
    fprintf( stderr, "Payload data from unexpected %s\n",
        ip_addr_text( sin ) );
    return;
  }

  //  Zero-terminate the string and tell user
  PayloadRequest & pr = *(PayloadRequest *)data;
  pr.message[ MAX_NAME_LEN-1 ] = 0;
  fprintf( stdout, "Received: %s from %s\n", pr.message, 
      ip_addr_text( sin ) );

  //  If this is "hello", then send "bye", else send "hello".
  if( !strcmp( pr.message, "hello" ) ) {
    strcpy( pr.message, "bye" );
    joiner_done = true;
  }
  else {
    strcpy( pr.message, "hello" );
  }
  //  Send to the host (presumably), UDP is lossy so I don't 
  //  check for errors here.
  ::sendto( gSocket, (char const *)&pr, sizeof( pr ), 0, 
      (sockaddr *)&sin, sizeof( sin ) );
}

//  Run the main loop for the peer that is joining. Participate in 
//  the simple hello/bye exchange defined by the hoster above, 
//  after being introduced by the introducer.
void run_joiner( char const * introducer, char const * name, char const * ip )
{
  //  Need a socket
  allocate_socket();
  //  Need an introducer
  gIntroAddr = resolve_text_addr( introducer );
  //  And need the host
  gHostAddr = resolve_numeric_addr( ip );

  while( !joiner_done ) {
    //  Form a registration packet; keep doing this until 
    //  we've actually registered.
    JoinRequest jr;
    jr.what = Request::JOIN;
    strncpy( jr.name, name, MAX_NAME_LEN );
    jr.name[ MAX_NAME_LEN-1 ] = 0;
    //  Send it (no error checking -- might get lost)
    fprintf( stderr, "Asking to be introduced to %s\n", 
        name );
    int r = ::sendto( gSocket, (char const *)&jr, sizeof( jr ), 
        0, (sockaddr *)&gIntroAddr, sizeof( gIntroAddr ) );
    maybe_reallocate_socket( r );

    //  Also send a "hello" payload data to the host
    PayloadRequest pr;
    pr.what = Request::PAYLOAD;
    strcpy( pr.message, "hello" );
    fprintf( stderr, "Sending hello to host %s\n",
        ip_addr_text( gHostAddr ) );
    r = ::sendto( gSocket, (char const *)&pr, sizeof( pr ), 0, 
        (sockaddr *)&gHostAddr, sizeof( gHostAddr ) );
    maybe_reallocate_socket( r );

    //  See if anything's waiting, else just delay for a while
    if( wait_for_socket( gSocket ) ) {
      sockaddr_in sin;
      socklen_t slen = sizeof( sin );
      char data[ 512 ];
      // Wait for a packet
      int r = ::recvfrom( gSocket, data, 512, 0, 
          (sockaddr *)&sin, &slen );
      if( maybe_reallocate_socket( r ) ) {
        continue;
      }

      // Dispatch based on the "what" code in the packet.
      Request & req = *(Request *)data;
      switch( req.what ) {
        case Request::PAYLOAD:
          joiner_payload( data, r, sin );
          break;
        default:
          fprintf( stderr, "Unknown what code: %d\n", req.what );
      }
    }
  }

  fprintf( stderr, "The joiner has hand-shaked with the host.\n" );
  exit( 0 );
}

