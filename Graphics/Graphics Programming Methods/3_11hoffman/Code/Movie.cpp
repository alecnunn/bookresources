#include "stdafx.h"
#include "Movie.h"
#include "Anim.h"
#include <assert.h>

Movie::Movie() 
{
	m_anims.clear();
	m_currentAnim = 0;
}

Movie::~Movie() 
{
	for (int i=0; i < m_anims.size(); i++)
	{
		Anim *a = m_anims[i];
		SAFE_DELETE(a);
	}
}

void Movie::AdvanceAnim()
{
	int num = GetNumAnims();
	m_currentAnim++;

	if (m_currentAnim == num) m_currentAnim = 0;
}

void Movie::Read2(FILE *f)
{
	int size;
	fscanf(f, "%d",&size); // num anims
	for (int i=0; i < size; i++)
	{
		Anim *anim  = new Anim;
		anim->Read2(f);

		AddAnim(anim);
	}

	if (size == 0) assert(false);

}


void Movie::Dump2(FILE *f)
{
	int size = m_anims.size();
	fprintf(f, "%d\n",size); // num anims
	for (int i=0; i < size; i++)
	{
		Anim *anim  = m_anims[i];
		anim->Dump2(f);
	}

}