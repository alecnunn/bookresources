//GPExample
//Copyright John Manslow
//29/09/2001

/////////////////////////////////////////////////////////////
//Only when compiling under Windows
#include "stdafx.h"
/////////////////////////////////////////////////////////////

#include "CWorld.h"
#include "assert.h"

CWorld::CWorld(const unsigned long ulNewWorldSizeX,
							const unsigned long ulNewWorldSizeY)
{
	unsigned long i;
	ulWorldSizeX=ulNewWorldSizeX;
	ulWorldSizeY=ulNewWorldSizeY;
	ppnWorld=new int*[ulWorldSizeX];
	for(i=0;i<ulWorldSizeX;i++)
	{
		ppnWorld[i]=new int[ulWorldSizeY];
	}
	Initialise();
}

CWorld::~CWorld()
{
	unsigned long i;
	for(i=0;i<ulWorldSizeX;i++)
	{
		delete []ppnWorld[i];
	}
	delete []ppnWorld;
}

void CWorld::Initialise(void)
{
	unsigned long i,j;
	for(i=0;i<ulWorldSizeX;i++)
	{
		for(j=0;j<ulWorldSizeY;j++)
		{
			ppnWorld[i][j]=0;
		}
	}
	for(i=0;i<ulWorldSizeX;i++)
	{
		ppnWorld[i][0]=2;
		ppnWorld[i][ulWorldSizeY-1]=2;
	}
	for(i=0;i<ulWorldSizeY;i++)
	{
		ppnWorld[0][i]=2;
		ppnWorld[ulWorldSizeX-1][i]=2;
	}
	unsigned long ulx,uly;
	for(i=0;i<600;i++)
	{
		ulx=rand()%(ulWorldSizeX-2)+1;
		uly=rand()%(ulWorldSizeY-2)+1;
		ppnWorld[ulx][uly]=1;
	}
	for(i=0;i< 400;i++)
	{
		ulx=rand()%(ulWorldSizeX-2)+1;
		uly=rand()%(ulWorldSizeY-2)+1;
		ppnWorld[ulx][uly]=3;
	}
	ulCharacterLocationX=rand()%(ulWorldSizeX-2)+1;
	ulCharacterLocationY=rand()%(ulWorldSizeY-2)+1;
}

void CWorld::Draw(CDC *pDC)
{
	unsigned long i,j;
	CBrush *pOldBrush=(CBrush*)pDC->SelectStockObject(BLACK_BRUSH);
	CPen *pOldPen=(CPen*)pDC->SelectStockObject(NULL_PEN);
	CBrush *pBrush;
	double dScale=400.0/double(ulWorldSizeX);
	for(i=0;i<ulWorldSizeX;i++)
	{
		for(j=0;j<ulWorldSizeY;j++)
		{
			pBrush=NULL;
			if(ppnWorld[i][j]==0)
			{
				pBrush=new CBrush(RGB(255,255,255));
			}
			if(ppnWorld[i][j]==2)
			{
				pBrush=new CBrush(RGB(0,0,0));
			}
			if(ppnWorld[i][j]==1)
			{
				pBrush=new CBrush(RGB(0,200,0));
			}
			if(ppnWorld[i][j]==3)
			{
				pBrush=new CBrush(RGB(255,0,0));
			}
			assert(pBrush);
			pDC->SelectObject(pBrush);
			pDC->Rectangle(int(i*dScale),int(j*dScale),int((i+1)*dScale+1),int((j+1)*dScale+1));
			pDC->SelectObject(pOldBrush);
			delete pBrush;
		}
	}
	pBrush=new CBrush(RGB(0,0,255));
	pDC->SelectObject(pBrush);
	pDC->Rectangle(
									int(ulCharacterLocationX*dScale),
									int(ulCharacterLocationY*dScale),
									int((ulCharacterLocationX+1)*dScale+1),
									int((ulCharacterLocationY+1)*dScale+1));
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
	delete pBrush;
}
