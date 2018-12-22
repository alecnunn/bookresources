#include "SelectionTree.hpp"



SelectionTree::SelectionTree() { 	
	m_pScenePatches = NULL;
	m_TreeElements = NULL;

	m_nScenePatches = m_nElements = 0;
}


SelectionTree::~SelectionTree()
{
	if (m_TreeElements)
		delete m_TreeElements;
}


void SelectionTree::Build(Patch* scenePatches, long nScenePatches)
{
	m_nScenePatches = nScenePatches;
	m_pScenePatches = scenePatches;

	// calculate the depth of the tree
    long j=m_nScenePatches;
	long log2=0;
	while (j >0) {
		log2++;
		j=j>>1;
	}

	long elements = (1 << log2) -1;
	if (m_nElements!=elements) {
		if (m_TreeElements) 
			delete m_TreeElements;
		m_nElements=elements;
		m_TreeElements = new TreeElement[m_nElements];
	}

	//reset the array by clearing the elements
	for (long i=0; i<m_nElements; i++) {
		m_TreeElements[i].clear();
	};

	long k=(1<<(log2-1))-1;

	TreeElement tmp1, tmp2;
	// calculate the leaves of the tree based on the patches
	for (i=0; i<m_nScenePatches; i+=2) {
		if (i<m_nScenePatches) {
			fillElement(i, tmp1);
		} else {
			tmp1.clear();
		}

		if (i+1<m_nScenePatches) {
			fillElement(i+1, tmp2);
		} else {
			tmp2.clear();
		}
		m_TreeElements[k++] = tmp1+tmp2;
	}

	// the leaves are done, build the tree!
	for (int l=log2-2; l>=0; l--) {
		long start=(1<<l)-1;
		long end=(1<<(l+1))-1;

		for (long m=start; m<end; m++) {
			m_TreeElements[m] = m_TreeElements[(m<<1)+1] + m_TreeElements[(m<<1)+2];
		}
	}
}


void SelectionTree::Update(long index)
{
	long l=(m_nElements+index-1)>>1;
	long index2 = index + ( index%2==0 ? 1 : -1 );

	TreeElement tmp1, tmp2;
	if (index<m_nScenePatches) {
		fillElement(index, tmp1);
	} else {
		tmp1.clear();
	}

	if (index2<m_nScenePatches) {
		fillElement(index2, tmp2);
	} else {
		tmp2.clear();
	}

	// calculate the leaf
	m_TreeElements[l] = tmp1+tmp2;

	// propagate the value up to the root
	for (l=(l-1)>>1; l>=0; l=(l-1)>>1) {
		m_TreeElements[l]=m_TreeElements[(l<<1)+1]+m_TreeElements[(l<<1)+2];
	}
}


long SelectionTree::FindDistrib(float p, long nIteration)
{
	p*=getRootValue(nIteration);

	// indx points to the left leaf
	for (long indx = 1; indx<m_nElements; indx = (indx<<1)+1) {
		const float left=getValue(m_TreeElements[indx], nIteration);
		if (p>left) {
			// go right
			indx++;
			p-=left;
		}
	}

	long l=indx-m_nElements;
	if (l>=m_nScenePatches) 
		return m_nScenePatches-1;

	//End of the tree; check the 2 leaves
	//i.e. check the left. If it is too small, pick the right one
	if (p > getValue2(l, nIteration)) l++;
	if (l>=m_nScenePatches) 
		return m_nScenePatches-1;
	else return l;
}

//-----------------------------------------
//-----------  PowerTree specific method --
//-----------------------------------------
void PowerTree::fillElement(long patchIndex, TreeElement& t)
{
	t.reflected=M_PI*m_pScenePatches[patchIndex].area*m_pScenePatches[patchIndex].Ld.Average();
	t.reflected+=m_pScenePatches[patchIndex].area*m_pScenePatches[patchIndex].C_I;
	
	if (m_pScenePatches[patchIndex].m_pMaterial->m_edf) {
		t.emitted=m_pScenePatches[patchIndex].m_pMaterial->m_edf->GetPowerOfArea(m_pScenePatches[patchIndex].area).Average();
	}
	else {
		t.emitted=0.0f;
	}
}

//------------------------------------------------------
//-----------  ImportanceTree specific method ----------
//------------------------------------------------------
void ImportanceTree::fillElement(long patchIndex, TreeElement& t) 
{ 
	if (m_pScenePatches[patchIndex].visible) {
		// t.emitted = average albedo
		t.emitted = m_pScenePatches[patchIndex].m_pMaterial->m_brdf->GetReflectanceLum(); 
	} else {
		t.emitted = 0.0f;
	}

	t.reflected = (M_PI*m_pScenePatches[patchIndex].Rd + m_pScenePatches[patchIndex].C_W); 
}
