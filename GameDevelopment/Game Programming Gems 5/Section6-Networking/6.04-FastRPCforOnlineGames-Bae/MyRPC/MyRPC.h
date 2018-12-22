// MyRPC helper library header file
//
// Copyright 2004 Bae,Hyun-jik.  All rights reserved.
//
// Permission is granted to use this code for any purpose, as long as this
// copyright message remains intact.

#pragma once

#include <vector>
#include <string>

/** RPC message identifier
\todo you may change this type to what you want */
typedef unsigned short RPCHeader; 

/** an identifier indicating send-to target
\todo you may change this type to what you want */
typedef unsigned int RPCSendTo;

/** additional send context identifier, for example, message protocol, asynchronous send notification.
\todo you may change this type to what you want */
typedef unsigned int RPCSendContext;

/** return values from RPC function invocation.
\todo you may change this type to what you want */
typedef unsigned int RPCResult;

/** means, "RPC function call successful" */
static const RPCResult RPCResult_Ok=0;

/** Simple message class.
CMessage works like a file, but it stores its data in a memory block. 
CMessage has 'cursor', which roles as a file pointer.

\todo This class may cause many memory fragmentations due to its internal
heap allocation. You may want to solve this problem by, for example, adding custom allocators or
allowing CMessage to receive external message buffer from your network engine. 

\todo This class does minimum error checking so you may add more code here. */
class CMessage
{
	unsigned char* m_buffer;
	int m_cursor;
public:
	CMessage(unsigned char* buffer);
	~CMessage();

	void Read(void* data,int dataLength);
	void Write(const void* data,int dataLength);
	void SetCursor(int cursor);
	inline int GetCursor() { return m_cursor; }
//	void CopyFrom(void* src,int srcLength);

	inline unsigned char* GetBuffer() { return m_buffer; }
//	inline int GetLength() { return (int)m_buffer.size(); }
};

// 
//These functions serialize a variable into CMessage object.
// 

inline void Message_Write(CMessage& m,const bool &val)
{
	m.Write(&val,sizeof(val));
}

inline void Message_Write(CMessage& m,const unsigned char &val)
{
	m.Write(&val,sizeof(val));
}

inline void Message_Write(CMessage& m,const char &val)
{
	m.Write(&val,sizeof(val));
}

inline void Message_Write(CMessage& m,const unsigned short &val)
{
	m.Write(&val,sizeof(val));
}

inline void Message_Write(CMessage& m,const short &val)
{
	m.Write(&val,sizeof(val));
}

inline void Message_Write(CMessage& m,const unsigned int &val)
{
	m.Write(&val,sizeof(val));
}

inline void Message_Write(CMessage& m,const int &val)
{
	m.Write(&val,sizeof(val));
}

inline void Message_Write(CMessage& m,const unsigned long &val)
{
	m.Write(&val,sizeof(val));
}

inline void Message_Write(CMessage& m,const long &val)
{
	m.Write(&val,sizeof(val));
}

inline void Message_Write(CMessage& m,const float &val)
{
	m.Write(&val,sizeof(val));
}

inline void Message_Write(CMessage& m,const double &val)
{
	m.Write(&val,sizeof(val));
}

inline void Message_Write(CMessage& m,const std::string &val)
{
	size_t length=val.size();
	Message_Write(m,length);
	m.Write(val.c_str(),(int)length);
}

inline void Message_Read(CMessage& m,bool &val)
{
	m.Read(&val,sizeof(val));
}

inline void Message_Read(CMessage& m,unsigned char &val)
{
	m.Read(&val,sizeof(val));
}

// 
//These functions deserialize a variable out of CMessage object.
// 

inline void Message_Read(CMessage& m,char &val)
{
	m.Read(&val,sizeof(val));
}

inline void Message_Read(CMessage& m,unsigned short &val)
{
	m.Read(&val,sizeof(val));
}

inline void Message_Read(CMessage& m,short &val)
{
	m.Read(&val,sizeof(val));
}

inline void Message_Read(CMessage& m,unsigned int &val)
{
	m.Read(&val,sizeof(val));
}

inline void Message_Read(CMessage& m,int &val)
{
	m.Read(&val,sizeof(val));
}

inline void Message_Read(CMessage& m,unsigned long &val)
{
	m.Read(&val,sizeof(val));
}

inline void Message_Read(CMessage& m,long &val)
{
	m.Read(&val,sizeof(val));
}

inline void Message_Read(CMessage& m,float &val)
{
	m.Read(&val,sizeof(val));
}

inline void Message_Read(CMessage& m,double &val)
{
	m.Read(&val,sizeof(val));
}

inline void Message_Read(CMessage& m,std::string &val)
{
	size_t length;
	Message_Read(m,length);
	val.resize(length,0);
	m.Read(&val[0],(int)length);
}

/** This function shall be called by MyRPC proxy. 
You should implement your own one. */
RPCResult RPC_Send(RPCSendTo sendTo,RPCSendContext sendContext,CMessage& m);
