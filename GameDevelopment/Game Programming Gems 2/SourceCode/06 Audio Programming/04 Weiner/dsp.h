/******************************************************************************
File: 		 dsp.h
Tab stops: every 2 columns
Project:	 Game Programming Gems II
Copyright: 2001 DiamondWare, Ltd.  Usage permitted for buyers of the book.
Written:	 by Keith Weiner
Purpose:	 Declares the interface to the DSP pipeline
History:	 02/20/01 KW Started
******************************************************************************/



#ifndef dsp_INCLUDE

	#define dsp_INCLUDE



	#include "dwdefs.h"



	#define dsp_FIRSTCALL  1
	#define dsp_LASTCALL	 2
	#define dsp_OUTPUT		 3
	#define dsp_QUERYACTIN 4



	typedef struct
	{
		/*------------------------------------------------------------------------
			Inputs
		------------------------------------------------------------------------*/
		UINT32 cmd; 						//command to dsp
		INT32 *lsrc;						//ptr to L input buffer
		INT32 *rsrc;						//ptr to R input buffer
		INT32 *ldst;						//ptr to L output buffer
		INT32 *rdst;						//ptr to R output buffer
		UINT32 srclen;					//number of samples in input buffer
		UINT32 dstlen;					//number of samples to write to output buffer
		void *dspspecific;		 //this is where the DSP-specific parms are passed
		/*------------------------------------------------------------------------
			Outputs
		------------------------------------------------------------------------*/
		UINT32 nowork;					//Filter returns !0 if it data should be unchanged
		UINT32 actualin;				//number of samples read from input buffer
		UINT32 actualout; 			//number of samples written to output buffer
		UINT32 dynrngfactor;		//parts per dws_IDENTITY of change in dynamic range
		/*------------------------------------------------------------------------
			For filter private use, malloc in dsp_FIRSTCALL, free in dsp_LASTCALL
		------------------------------------------------------------------------*/
		void *privdata; 				//per snd data to be kept persistent for the DSP

	} dsp_PARAMS;


	typedef void (dsp_CALLBACK)(dsp_PARAMS *params);



	typedef struct
	{
		dsp_CALLBACK *callback;
		bfbuf_BUFFER *underbuf;

		UINT32 inreq;
		UINT32 outreq;

		void *dspspecific;
		void *privdata;

	} dsp_DSP;


	typedef struct
	{
		UINT32 src;
		UINT32 dst;
		UINT32 acc;

		UINT32 samps;
		UINT32 sampsneeded;
		UINT32 dstindex;
		UINT32 done;

		UINT32 numdsps;
		dsp_DSP *dsp;

		bfbuf_BUFFER buf[3];

	} dsp_AUDIO;



	void dsp_NewSound(dsp_AUDIO *audio);

	void dsp_DeleteSound(dsp_AUDIO *audio);


	void dsp_ProcessAudio(dsp_AUDIO *audio);



#endif
