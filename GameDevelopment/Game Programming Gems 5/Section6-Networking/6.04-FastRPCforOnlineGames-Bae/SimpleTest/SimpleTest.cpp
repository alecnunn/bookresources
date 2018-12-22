// SimpleTest.cpp : Defines the entry point for the console application.
//
// Copyright 2004 Bae,Hyun-jik.  All rights reserved.
//
// Permission is granted to use this code for any purpose, as long as this
// copyright message remains intact.

#include "stdafx.h"
#include "IncludeIDL.h"

unsigned char g_simMsgBuffer[1024]; // buffer of a simulated message
CMessage g_simulateTransmission(g_simMsgBuffer);

// A fake networking function. This simulates sending a message. It actually stores the message into a varaiable :)
RPCResult RPC_Send(RPCSendTo sendTo,RPCSendContext sendContext,CMessage &m)
{
	memcpy(g_simMsgBuffer,m.GetBuffer(),m.GetCursor());
	return RPCResult_Ok;
}

// the simplest use of RPC system
int main(int argc, _TCHAR* argv[])
{
	Vector3D p,v;
	p.x=1; p.y=2; p.z=3;
	v.x=4; v.y=5; v.z=6;

	cout<<"Calling an RPC via proxy which generates a message...\n";
	TestRPCProxy::Knight_Move(1,2,3,4,5,6);
	cout<<"Calling the stub with the generated message...\n";
	TestRPCStub::RPC_DoStub(3,4,g_simulateTransmission);  // A fake of networking: Receive a message and process it :)

	cout<<endl;

	cout<<"Calling an RPC via proxy which generates a message...\n";
	TestRPCProxy::ShowChat(1,2,"Hello, RPC world.");
	cout<<"Calling the stub with the generated message...\n";
	TestRPCStub::RPC_DoStub(3,4,g_simulateTransmission); // A fake of networking: Receive a message and process it :)

	return 0;
}

// an example RPC body function
RPCResult TestRPCStub::Knight_Move(RPCSendTo sendTo,RPCSendContext sendContext,const int &id,const float &x,const float &y,const float &z)
{
	cout<<"Knight_Move ok, id="<<id<<",x="<<x<<",y="<<y<<",z="<<z<<endl;
	return RPCResult_Ok;
}

// an example RPC body function
RPCResult TestRPCStub::Knight_Attack(RPCSendTo sendTo,RPCSendContext sendContext,const int &id,const int &target,const int &damage)
{
	return RPCResult_Ok;
}

// an example RPC body function
RPCResult TestRPCStub::ShowChat(RPCSendTo sendTo,RPCSendContext sendContext,const string &text)
{
	cout<<"ShowChat ok, text="<<text<<endl;
	return RPCResult_Ok;
}

