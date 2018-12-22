/******************************************************************************
File: 		 bfbuf.h
Tab stops: every 2 columns
Project:	 Game Programming Gems II
Copyright: 1998-2000 DiamondWare, Ltd.	Usage permitted for buyers of the book.
Written:	 by Keith Weiner
Purpose:	 Contains the BackFill Buffer manager
History:	 11/20/98 KW Started
					 02/20/01 KW modified (simplified) for book
******************************************************************************/



#include <stdlib.h>

#include "bfbuf.h"



void bfbuf_Eat(bfbuf_BUFFER *buf, UINT32 len)
{
	buf->datalen -= len;
}


void bfbuf_ChokeUp(bfbuf_BUFFER *buf, UINT32 newlen)
{
	UINT32 *lsrc;
	UINT32 *rsrc;
	UINT32 *ldst;
	UINT32 *rdst;

	if (buf->datalen - newlen)
	{
		bfbuf_GetBufStart(buf, &lsrc, &rsrc);

		buf->datalen = newlen;

		if (newlen)
		{
			bfbuf_GetBufStart(buf, &ldst, &rdst);

			memmove(ldst, lsrc, newlen * sizeof(*ldst));
			memmove(rdst, rsrc, newlen * sizeof(*rdst));
		}
	}
}


UINT32 bfbuf_MoveAll(bfbuf_BUFFER *dst, bfbuf_BUFFER *src)
{
	return (bfbuf_Move(dst, src, src->datalen));
}


UINT32 bfbuf_Move(bfbuf_BUFFER *dst, bfbuf_BUFFER *src, UINT32 len)
{
	UINT32 *lsrc;
	UINT32 *rsrc;
	UINT32 datalen;
	UINT32 buflen;
	UINT32 *ldst;
	UINT32 *rdst;
	UINT32 *lnew;
	UINT32 *rnew;

	if (!src->datalen)
	{
		return (bfbuf_SUCCESS); 				//okay...
	}

	datalen = dst->datalen + len; 		//new, final datalen in UINT32's

	if (dst->buflen < datalen)
	{
		/* New data won't fit into dst buffer, so use 125% of datalen */
		buflen = datalen + (datalen >> 2);

		/* We allocate new buffers (realloc wouldn't help a bfbuf anyways) */
		if ((lnew = malloc(buflen * sizeof(*lnew) * 2)) == NULL)
		{
			return (bfbuf_FAILURE);
		}

		if ((rnew = malloc(buflen * sizeof(*rnew) * 2)) == NULL)
		{
			free(lnew);

			return (bfbuf_FAILURE);
		}

		if (dst->buflen)
		{
			/* The old buffer was not NULL */

			if (dst->datalen)
			{
				/* The old buffer had data in it */
				ldst = lnew + buflen - dst->datalen;
				rdst = rnew + buflen - dst->datalen;

				bfbuf_GetBufStart(dst, &lsrc, &rsrc);

				memcpy(ldst, lsrc, dst->datalen * sizeof(*ldst));
				memcpy(rdst, rsrc, dst->datalen * sizeof(*rdst));
			}

			free(dst->lbuf);
			free(dst->rbuf);
		}

		dst->buflen = buflen;
		dst->lbuf 	= lnew;
		dst->rbuf 	= rnew;
	}

	dst->datalen = datalen;

	bfbuf_GetBufStart(src, &lsrc, &rsrc);
	bfbuf_GetBufStart(dst, &ldst, &rdst);

	/* Move data from src to dst */
	memcpy(ldst, lsrc, len * sizeof(*ldst));
	memcpy(rdst, rsrc, len * sizeof(*rdst));

	src->datalen -= len;

	return (bfbuf_SUCCESS);
}


void bfbuf_SetDataLen(bfbuf_BUFFER *buf, UINT32 len)
{
	UINT32 *tmp;

	buf->datalen = len;

	if (buf->buflen < buf->datalen)
	{
		if ((buf->buflen) && (buf->lbuf) != NULL)
		{
			free(buf->lbuf);
		}

		if ((buf->buflen) && (buf->rbuf) != NULL)
		{
			free(buf->rbuf);
		}

		buf->buflen = buf->datalen + (buf->datalen >> 2); 	//extra safety margin

		if ((buf->lbuf = malloc(buf->buflen * sizeof(*buf->lbuf) * 2)) != NULL)
		{
			if ((buf->rbuf = malloc(buf->buflen * sizeof(*buf->rbuf) * 2)) == NULL)
			{
				free(buf->lbuf);
			}
		}
	}
}


void bfbuf_GetBufStart(bfbuf_BUFFER *buf, UINT32 **left, UINT32 **right)
{
	*left  = buf->lbuf + (buf->buflen - buf->datalen);
	*right = buf->rbuf + (buf->buflen - buf->datalen);
}


void bfbuf_EndBuf(bfbuf_BUFFER *buf)
{
	if (buf->lbuf != NULL)
	{
		free(buf->lbuf);
	}

	if (buf->rbuf != NULL)
	{
		free(buf->rbuf);
	}
}


UINT32 bfbuf_BeginBuf(bfbuf_BUFFER *buf, UINT32 buflen)
{
	UINT32 len;

	len = buflen * (sizeof(*(buf->lbuf)));	//my kingdom for a typeof operator!

	if (len)
	{
		if ((buf->lbuf = malloc(len * 2)) == NULL)
		{
			return (bfbuf_FAILURE);
		}

		if ((buf->rbuf = malloc(len * 2)) == NULL)
		{
			free(buf->lbuf);

			return (bfbuf_FAILURE);
		}
	}
	else
	{
		buf->lbuf = NULL;
		buf->rbuf = NULL;
	}

	buf->buflen  = buflen;
	buf->datalen = 0;

	return (bfbuf_SUCCESS);
}
