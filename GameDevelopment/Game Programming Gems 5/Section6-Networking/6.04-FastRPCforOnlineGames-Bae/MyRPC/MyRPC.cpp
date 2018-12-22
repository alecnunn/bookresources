// MyRPC helper library
//
// Copyright 2004 Bae,Hyun-jik.  All rights reserved.
//
// Permission is granted to use this code for any purpose, as long as this
// copyright message remains intact.

#include "StdAfx.h"
#include ".\myrpc.h"

/** reads from message buffer */
void CMessage::Read(void* data,int dataLength)
{
	memcpy(data,&m_buffer[m_cursor],dataLength);
	m_cursor+=dataLength;
}

/** writes to message buffer */
void CMessage::Write(const void* data,int dataLength)
{
	memcpy(&m_buffer[m_cursor],data,dataLength);
	m_cursor+=dataLength;
}

CMessage::CMessage(unsigned char* buffer)
{
	m_buffer=buffer;
	m_cursor=0;
}

CMessage::~CMessage()
{
}

/** sets read/write position (cursor) to a specified one. */
void CMessage::SetCursor(int cursor)
{
	m_cursor=cursor;
}

/** takes a memory block and stores. */
/*void CMessage::CopyFrom(void* src,int srcLength)
{
	m_cursor=0;
	m_buffer.resize(srcLength);
	memcpy(&m_buffer[0],src,srcLength);
}*/
