#ifndef _MOVIE_H
#define _MOVIE_H

#include <stdio.h>
#include <vector>

class Anim;

class Movie {
public:
	Movie();
	~Movie();

	inline void AddAnim(Anim *a) {m_anims.push_back(a);};
	inline int GetNumAnims() {return m_anims.size();};
	inline Anim *GetCurrentAnim() {return m_anims[m_currentAnim];};

	void Read2(FILE *f);
	void Dump2(FILE *f);

	void AdvanceAnim();
private:
	inline Anim *GetAnim(int i) {return m_anims[i];};


private:
	std::vector<Anim *> m_anims;
	int m_currentAnim;

};

#endif

