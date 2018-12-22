#pragma once

#include "scene.hpp"

//! TreeElement class will be the element of the SelectionTree,
//! allowing radiance/importance-based patch selection
class TreeElement {
public:
	float emitted, reflected;
	TreeElement() {};
	TreeElement(float e, float r) : emitted(e), reflected(r) {};
	void set(float e, float r) { emitted=e; reflected=r; };
	TreeElement operator+(const TreeElement& o) const {
		return TreeElement(emitted+o.emitted, reflected+o.reflected);
	}
	const TreeElement& operator=(const TreeElement& o) {
		emitted=o.emitted;
		reflected=o.reflected;
		return *this;
	}
	void clear() { emitted = reflected =0.0f; };
};

//! SelectionTree allows radiance/importance-based 
//! patch selection and update in O(log n)
class SelectionTree {
protected:
	TreeElement* m_TreeElements;
	long m_nElements;

	long m_nScenePatches;
	Patch* m_pScenePatches;

public:
	SelectionTree();
	~SelectionTree();

	void Build(Patch* scenePatches, long nScenePatches);
	void Update(long index);
	long FindDistrib(float p, long nIteration);

	virtual void fillElement(long patchIndex, TreeElement& t)=0;
	
	float getValue(const TreeElement& t, long nIteration) {
		return t.emitted + t.reflected/(float)nIteration;
	}

	float getValue2(long patchIndex, long nIteration) {
		TreeElement t;
		fillElement(patchIndex, t);
		return getValue(t, nIteration);
	};

	float getRootValue(long nIteration) { return getValue(m_TreeElements[0], nIteration); }
};

class PowerTree : public SelectionTree {
public:
	virtual void fillElement(long patchIndex, TreeElement& t);
};


class ImportanceTree : public SelectionTree {
public:
	virtual void fillElement(long patchIndex, TreeElement& t);
};

