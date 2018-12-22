/*****************************************************************************/
/*                                                                           */
/*                    S  e  n  s  a  u  r  a     L  t  d                     */
/*                    ==================================                     */
/*                                                                           */
/*                            (C) Copyright 1999                             */
/*                                                                           */
/*****************************************************************************/
/*									     */
/*									     */
/*      Module name:    vmanpset.h		                             */
/*									     */
/*	Module number:	xxxxxxxx		Version:	1	     */
/*									     */
/*      Family name:    crlds3d		                                     */
/*									     */
/*      Creation date:  23/04/98                Revision date:  xx/xx/xx     */
/*									     */
/*	Produced by:	Peter J.C.Clare					     */
/*									     */
/*									     */
/*      Abstract:                                                            */
/*                                                                           */
/*		DirectSound3D Voice Manager property set definitions.	     */
/*									     */
/*****************************************************************************/
/*									     */
/*	Revision history						     */
/*	================						     */
/*									     */
/*	Date	    Vsn.    Initials	Description			     */
/*	~~~~	    ~~~~    ~~~~~~~~	~~~~~~~~~~~			     */
/*									     */
/*****************************************************************************/


#ifndef	_VMANPSET_H_
#define _VMANPSET_H_


/*****************************************************************************/
/*									     */
/*	G l o b a l   t y p e d e f s					     */
/*									     */
/*****************************************************************************/


typedef enum 
{
	DSPROPERTY_VMANAGER_MODE = 0,
	DSPROPERTY_VMANAGER_PRIORITY,
	DSPROPERTY_VMANAGER_STATE
} DSPROPERTY_VMANAGER;


typedef enum 
{
	DSPROPERTY_VMANAGER_MODE_DEFAULT = 0,
	DSPROPERTY_VMANAGER_MODE_AUTO,
	DSPROPERTY_VMANAGER_MODE_REPORT,
	DSPROPERTY_VMANAGER_MODE_USER
} VmMode;


typedef enum 
{
	DSPROPERTY_VMANAGER_STATE_PLAYING3DHW = 0,
	DSPROPERTY_VMANAGER_STATE_SILENT,
	DSPROPERTY_VMANAGER_STATE_BUMPED,
	DSPROPERTY_VMANAGER_STATE_PLAYFAILED
} VmState;




/*****************************************************************************/
/*									     */
/*	P u b l i c   v a r i a b l e s					     */
/*									     */
/*****************************************************************************/


#ifdef	__cplusplus
extern	"C"
{
#endif

/* {62A69BAE-DF9D-11D1-99A6-00C04FC99D46} */
DEFINE_GUID( DSPROPSETID_VoiceManager, 0x62A69BAE, 0xDF9D, 0x11D1, 0x99, 0xA6, 0x00, 0xC0, 0x4F, 0xC9, 0x9D, 0x46 );

#ifdef	__cplusplus
}
#endif

#endif
