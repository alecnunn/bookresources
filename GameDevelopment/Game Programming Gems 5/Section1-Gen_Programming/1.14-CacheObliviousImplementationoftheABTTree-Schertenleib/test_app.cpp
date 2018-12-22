////////////////////////////////////////////////////////////////////////////
//	Copyright : Sebastien Schertenleib
//	          : VRLab-EPFL
//
//	Email :Sebastien.Schertenleib@epfl.ch
//
//	This source code is a part of the Game Programming Gems 5 Source Code
//
//  Permission to copy, modify, reproduce or redistribute this source code is
//  granted provided the above copyright notice is retained in the resulting 
//  source code.
//
//	This file is provided 'as is' with no expressed or implied warranty.
//	The author accepts no liability if it causes any damage to your computer.
//
//	Do expect bugs.
//	Please let me know of any bugs/mods/improvements.
//	and I will try to fix/incorporate them into this file.
//	Enjoy!
//
////////////////////////////////////////////////////////////////////////////


#include <iostream>
#include "vhdABTTree.h"
#include "vhdSceneConverterABTTree.h"




#define NB_OBJECTS 3202

unsigned int g_uiNumberOfVisibleLeaf=0;

void 
_ABTTreeVisit(unsigned int uiCurrentPos, unsigned int uiIndex, const vhdAxisAlignedBox &bbox)
{
	//the leaf is visible
	std::cout << "Leaf at Position            : " << uiCurrentPos << std::endl;
	std::cout << "Leaf Index                  : " << uiIndex  << std::endl;
	std::cout << "Lead ID                     : "; 
	//loop
	vhdSceneConverterABTTree::ListAABB::iterator it = vhdSceneConverterABTTree::getSingleton().mapNodeChild[uiCurrentPos].listAABB.begin();
	std::cout << it->strPath;
	++it;
	for ( it; it!= vhdSceneConverterABTTree::getSingleton().mapNodeChild[uiCurrentPos].listAABB.end(); ++it)
	{
		std::cout << " , " << it->strPath;
	}
	std::cout << std::endl << std::endl;
	
	g_uiNumberOfVisibleLeaf+=(unsigned int) vhdSceneConverterABTTree::getSingleton().mapNodeChild[uiCurrentPos].listAABB.size();
}


bool _ABTTreeIsVisibleVisit (const vhdAxisAlignedBox &bbox)
{
	//In real world, you will test if the bbox is contains in your view of interest
	return true;
}

bool _ABTTreeIsVisibleVisitRandom (const vhdAxisAlignedBox &bbox)
{

	//assure some parsing
	static unsigned int uiAss(0);
	if (++uiAss%10 == 0)
	{
		return false;
	}
	return true;
}

vhdSceneConverterABTTree singl;
int main()
{
	vhdABTTree *pABTTree = new vhdABTTree();

	vhdSceneConverterABTTree::vhdSceneABTTreeNode node;
	
	
	//we build the number of object we would like to try!
	unsigned int i(0);
	for ( i ; i < NB_OBJECTS ; ++i )
	{
		vhdAxisAlignedBox axis(i*100, i*100,i*100,i*100+50,i*100+50,i*100+50);
		char buf[256];
		sprintf(buf,"%d",i);
		std::string str=std::string("Leaf Id ")+std::string(buf);

		node.listAABB.push_back(vhdSceneConverterABTTree::NodeChildStruct(axis,str));
	}
	vhdSceneConverterABTTree::getSingleton().pDataRootNode = &node;
	
	
	pABTTree->create(NB_OBJECTS);
	pABTTree->funcIsVisible = _ABTTreeIsVisibleVisit;
	pABTTree->funcVisit = _ABTTreeVisit;

	std::cout << std::endl << std::endl << std::endl << std::endl;
	std::cout << "Complete Search" << std::endl << std::endl;
	g_uiNumberOfVisibleLeaf=0;
	pABTTree->traversal();
	std::cout << std::endl << "Complete Search: number of leaves visited " << g_uiNumberOfVisibleLeaf << std::endl << std::endl << std::endl;

	std::cout << std::endl << std::endl << std::endl << std::endl;
	std::cout << "Random Search" << std::endl << std::endl;
	//we try the random isVisible Routine
	pABTTree->funcIsVisible = _ABTTreeIsVisibleVisitRandom;
	g_uiNumberOfVisibleLeaf=0;
	pABTTree->traversal();
	std::cout << std::endl << "Random Search : number of leaves visited " << g_uiNumberOfVisibleLeaf << std::endl << std::endl << std::endl;

	//put the complete search back
	pABTTree->funcIsVisible = _ABTTreeIsVisibleVisit;
	std::cout << std::endl << std::endl << std::endl << std::endl;
	std::cout << "Complete Search" << std::endl << std::endl;
	g_uiNumberOfVisibleLeaf=0;
	pABTTree->traversal();
	std::cout << std::endl << "Complete Search: Number of Leaves Visited " << g_uiNumberOfVisibleLeaf << std::endl << std::endl << std::endl;


	pABTTree->destroy();
	delete pABTTree;
}

