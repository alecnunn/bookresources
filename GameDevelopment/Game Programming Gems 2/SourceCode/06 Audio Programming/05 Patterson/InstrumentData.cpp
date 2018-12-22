/* Copyright (C) Scott Patterson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2001"
 */

// InstrumentData.cpp

#include "StdAfx.h"
#include "csynutil.h"
#include "InstrumentData.h"

InstrumentData_t::InstrumentData_t()
{
	m_nID = 0;
	m_nType = 0;
	m_CSynSound = 0;
}

InstrumentData_t::~InstrumentData_t()
{
	if( m_CSynSound ) CSynSound_Delete( m_CSynSound );
}

void InstrumentData_t::SetFileName( String_t &sFileName )
{
	m_sFileName = sFileName;
}

String_t InstrumentData_t::GetFileName()
{
	return m_sFileName;
}

void InstrumentData_t::SetID( int nID )
{
	m_nID = nID;
}

int InstrumentData_t::GetID()
{
	return m_nID;
}

void InstrumentData_t::SetType( int nType )
{
	m_nType = nType;
}

int InstrumentData_t::GetType()
{
	return m_nType;
}

void InstrumentData_t::SetCSynSound( CSynSound csynSnd )
{
	m_CSynSound = csynSnd;
}

CSynSound InstrumentData_t::GetCSynSound()
{
	return m_CSynSound;
}

void InstrumentData_t::SetSampleInfoPtr( AudioSample *pAudioSample )
{
	m_pAudioSample = pAudioSample;
}

AudioSample *InstrumentData_t::GetSampleInfoPtr()
{
	return m_pAudioSample;
}

