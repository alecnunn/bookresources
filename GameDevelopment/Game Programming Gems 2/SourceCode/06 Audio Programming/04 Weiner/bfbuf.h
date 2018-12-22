/******************************************************************************
File: 		 bfbuf.h
Tab stops: every 2 columns
Project:	 Game Programming Gems II
Copyright: 1998-2000 DiamondWare, Ltd.	Usage permitted for buyers of the book.
Written:	 by Keith Weiner
Purpose:	 Contains prototypes and typedefs for the BackFill Buffer manager
History:	 11/20/98 KW Started
					 02/20/01 KW modified (simplified) for book
******************************************************************************/



/*
 . New concept in data-structures: BackFill Buffer
 .
 . The buffer has a size and a block of data whose length is <= its size.
 . The data is always "choked up" to the end of the buffer.  To prepend
 . data (the operation that turns out to be most important in WIN-STK), we
 . simply copy data up to abutt the beginning of data.	To delete data off
 . the beginning of the block (i.e. when it's eaten), we change the length
 . value.  The actual start of data is given by:
 .	 datastart = bufadr + buflen - datalen
 .
 . It's not very complex to code. :)
*/



#ifndef bfbuf_INCLUDE

	#define bfbuf_INCLUDE



	#include "dwdefs.h"



	#define bfbuf_FAILURE 1
	#define bfbuf_SUCCESS 1



	typedef struct
	{
		UINT32 buflen;
		UINT32 datalen;
		UINT32 *lbuf;
		UINT32 *rbuf;

	} bfbuf_BUFFER;



	#define bfbuf_WIPE(buf) buf->datalen = 0

	#define bfbuf_GETDATALEN(buf) ((buf)->datalen)

	#define bfbuf_ISVALID(buf) ((buf)->datalen <= (buf)->buflen)



	UINT32 bfbuf_BeginBuf(bfbuf_BUFFER *buf, UINT32 buflen);

	void bfbuf_EndBuf(bfbuf_BUFFER *buf);


	void bfbuf_GetBufStart(bfbuf_BUFFER *buf, UINT32 **left, UINT32 **right);


	void bfbuf_SetDataLen(bfbuf_BUFFER *buf, UINT32 len);


	UINT32 bfbuf_Move(bfbuf_BUFFER *dst, bfbuf_BUFFER *src, UINT32 len);

	UINT32 bfbuf_MoveAll(bfbuf_BUFFER *dst, bfbuf_BUFFER *src);


	void bfbuf_ChokeUp(bfbuf_BUFFER *buf, UINT32 newlen);


	void bfbuf_Eat(bfbuf_BUFFER *buf, UINT32 len);



#endif
