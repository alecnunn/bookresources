// scene
// (c) Kenny Mitchell - Westwood Studios EA 2002

#ifndef _SCENE_H
#define _SCENE_H

class HeightPatch;
class Sky;
class Earth;

class Scene
{
public:
	Scene();
	~Scene();

	void Render();
	Sky *GetSky() {return m_pSky;};

private:

	HeightPatch*	m_pTerrain;
	Sky *m_pSky;
};

#endif
