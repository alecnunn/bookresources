/* Copyright (C) Scott Patterson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2001"
 */

// InstrumentData.h

#include "audio_sample.h"

class InstrumentData_t {
public:
	InstrumentData_t();
	~InstrumentData_t();

	void SetFileName( String_t &sFileName );
	String_t GetFileName();

	void SetID( int nID );
	int GetID();

	void SetType( int nType );
	int GetType();

	void SetCSynSound( CSynSound csynSnd );
	CSynSound GetCSynSound();

	void SetSampleInfoPtr( AudioSample *pAS );
	AudioSample *GetSampleInfoPtr();
private:
	String_t m_sFileName;
	int m_nID;
	int m_nType;
	CSynSound m_CSynSound;
	AudioSample *m_pAudioSample;
};

