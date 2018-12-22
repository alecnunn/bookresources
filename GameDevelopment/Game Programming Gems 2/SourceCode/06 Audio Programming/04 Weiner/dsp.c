/******************************************************************************
File: 		 dsp.c
Tab stops: every 2 columns
Project:	 Game Programming Gems II
Copyright: 2001 DiamondWare, Ltd.  Usage permitted for buyers of the book.
Written:	 by Keith Weiner
Purpose:	 Contains the implementation of the DSP pipeline
History:	 02/20/01 KW Started based on dsp.c from WIN-STK
******************************************************************************/



#include "bfbuf.h"
#include "dsp.h"



static void QueryDSPIn(dsp_AUDIO *audio)
{
	dsp_PARAMS params;
	dsp_DSP *dsp;
	UINT32 underlen;
	UINT32 x;

	params.cmd			= dsp_QUERYACTIN;
	params.actualin = audio->sampsneeded - audio->samps;

	for (x=0;x<audio->numdsps;x++)
	{
		dsp = &(audio->dsp[audio->numdsps - x - 1]);		//traverse chain backwards

		params.dstlen 		 = params.actualin;
		params.dspspecific = dsp->dspspecific;
		params.privdata 	 = dsp->privdata;

		(*dsp->callback)(&params);

		underlen = bfbuf_GETDATALEN(dsp->underbuf);

		dsp->inreq	= (underlen < params.actualin) ? params.actualin - underlen : 0;
		dsp->outreq = params.dstlen;
	}
}


static void DoDSP(dsp_AUDIO *audio)
{
	bfbuf_BUFFER *srcbuf;
	bfbuf_BUFFER *dstbuf;
	dsp_PARAMS params;
	dsp_DSP *dsp;
	UINT32 x;

	params.cmd = dsp_OUTPUT;

	for (x=0;x<audio->numdsps;x++)
	{
		dsp = &(audio->dsp[x]);

		SWAP(audio->src, audio->dst);

		srcbuf = &(audio->buf[audio->src]);
		dstbuf = &(audio->buf[audio->dst]);

		bfbuf_MoveAll(srcbuf, dsp->underbuf);
		bfbuf_SetDataLen(dstbuf, dsp->outreq);
		bfbuf_GetBufStart(srcbuf, &(params.lsrc), &(params.rsrc));
		bfbuf_GetBufStart(dstbuf, &(params.ldst), &(params.rdst));

		params.srclen 		 = bfbuf_GETDATALEN(srcbuf);
		params.dstlen 		 = bfbuf_GETDATALEN(dstbuf);
		params.dspspecific = dsp->dspspecific;
		params.privdata 	 = dsp->privdata;

		(*dsp->callback)(&params);

		if (params.nowork)
		{
			bfbuf_WIPE(dstbuf);

			SWAP(audio->src, audio->dst);

			params.actualout = params.srclen;
		}
		else
		{
			if (params.actualout < bfbuf_GETDATALEN(dstbuf))
			{
				bfbuf_ChokeUp(dstbuf, params.actualout);
			}

			if (params.actualin < bfbuf_GETDATALEN(srcbuf))
			{
				bfbuf_Eat(srcbuf, params.actualin);
				bfbuf_MoveAll(dsp->underbuf, srcbuf);
			}
			else
			{
				bfbuf_WIPE(srcbuf);
			}
		}

	}

	audio->samps += params.actualout;
}


void dsp_ProcessAudio(dsp_AUDIO *audio)
{
	audio->src=0;
	audio->dst=1;
	audio->acc=2;

	bfbuf_WIPE((bfbuf_BUFFER *)(&(audio->buf[audio->src])));
	bfbuf_WIPE((bfbuf_BUFFER *)(&(audio->buf[audio->dst])));
	bfbuf_WIPE((bfbuf_BUFFER *)(&(audio->buf[audio->acc])));

	audio->samps = 0;

	while ((audio->samps < audio->sampsneeded) && (!audio->done))
	{
		SWAP(audio->acc, audio->dst);

		QueryDSPIn(audio);
		stream_GetData(audio);
		DoDSP(audio);

		bfbuf_MoveAll(&(audio->buf[audio->dst]), &(audio->buf[audio->acc]));
	}
}


void dsp_NewSound(dsp_AUDIO *audio)
{
	dsp_PARAMS params;
	dsp_DSP *dsp;
	UINT32 x;

	for (x=0;x<audio->numdsps;x++)
	{
		dsp = &(audio->dsp[x]);

		params.cmd				 = dsp_FIRSTCALL;
		params.dspspecific = dsp->dspspecific;

		(*dsp->callback)(&params);

		dsp->privdata = params.privdata;
	}
}


void dsp_DeleteSound(dsp_AUDIO *audio)
{
	dsp_PARAMS params;
	dsp_DSP *dsp;
	UINT32 x;

	for (x=0;x<audio->numdsps;x++)
	{
		dsp = &(audio->dsp[x]);

		params.cmd				 = dsp_LASTCALL;
		params.dspspecific = dsp->dspspecific;
		params.privdata 	 = dsp->privdata;

		(*dsp->callback)(&params);
	}
}
