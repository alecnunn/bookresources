/*
Copyright (c) 2001 by Justin Randall.  All rights reserved.
This copyright applies only to this particular software distribution 
package, and does not imply a copyright on any particular module in 
the library.

Permission to use, copy, modify, and distribute this compilation for
non-commercial use is hereby granted without fee, subject to 
the following license:

1. Any copy or modification of this compilation must include the above
copyright notice and this license.

2. Users of the software included in this compilation agree to use
their best efforts to provide Justin Randall with any modifications containing
improvements or extensions and hereby grant Justin Randall a perpetual,
royalty-free license to use and distribute such modifications under
the terms of this license.

3. Justin Randall makes no warranty or representation that the operation of the
software in this compilation will be error-free, and Justin Randall is under no
obligation to provide any services, by way of maintenance, update, or
otherwise.  THE SOFTWARE AND ANY DOCUMENTATION ARE PROVIDED 'AS IS'
WITHOUT EXPRESS OR IMPLIED WARRANTY INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE. IN NO EVENT WILL JUSTIN RANDALL OR ANY OTHER CONTRIBUTOR BE LIABLE FOR
DIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

4. Users will not use Justin Randall or any other contributor's name in any 
publicity or advertising, without prior written consent in each case.

5. Export of this software from the United States may require a
specific license from the United States Government.  It is the
responsibility of any person or organization contemplating export
to obtain such a license before exporting.

6. Certain parts of this software may be protected by patents.  It
is the users' responsibility to obtain the appropriate
licenses before using those parts.
*/

#include "NetworkPCH.h"
#include "Archive/Archive.h"
#include "TcpClientSock.h"
#include "TcpServerSock.h"
#include "TcpSock.h"
#include "UpdateChain/UpdateChain.h"

//---------------------------------------------------------------------

namespace Network {

//---------------------------------------------------------------------

TcpSock::Packet::Packet() :
length(0),
data()
{
}

//---------------------------------------------------------------------

TcpMessage::TcpMessage(const TcpSock & s, const Archive::Stream::ReadIterator & d) :
data(d),
source(s)
{
}

//---------------------------------------------------------------------

class TcpSockUpdater : public UpdateChain::UpdateChain::UpdateObject
{
public:
	TcpSockUpdater();
	~TcpSockUpdater();
	void update();
};

//---------------------------------------------------------------------

TcpSockUpdater::TcpSockUpdater() :
UpdateObject()
{
}

//---------------------------------------------------------------------

TcpSockUpdater::~TcpSockUpdater()
{
}

//---------------------------------------------------------------------

void TcpSockUpdater::update()
{
	TcpClientSock::update();
	TcpServerSock::update();
}

//---------------------------------------------------------------------

TcpSock::TcpSock() :
group(TcpSock::getGroup())
{
}

//---------------------------------------------------------------------

TcpSock::TcpSock(const Address & l) :
Sock(l),
group(TcpSock::getGroup())
{
}

//---------------------------------------------------------------------

TcpSock::~TcpSock()
{
}

//---------------------------------------------------------------------

ps_group TcpSock::getGroup()
{
	static TcpSockUpdater u;
	static Sock::SockGroup s;
	return s.getGroup();
}

//---------------------------------------------------------------------

void TcpSock::onClosed()
{
	connected = false;
}

//---------------------------------------------------------------------

void TcpSock::onOpened()
{
	connected = true;
}

//---------------------------------------------------------------------

}//namespace Network
