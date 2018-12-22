/*******************************************************************
* ZFXEngine!                                                      *
* (c)2003 by Stefan Zerbst | www.zfx.info                         *
*-----------------------------------------------------------------*
* File: ZFXPolylist.cpp                                           *
* part of static math lib implementing basic 3D math objects      *
*******************************************************************/

// I N C L U D E S /////////////////////////////////////////////////

#include <stdlib.h>     // malloc, free, realloc
#include "zfx3d.h"      // class definitions and stuff


// F U N C T I O N S ///////////////////////////////////////////////


/**
 * Constructor
 */
ZFXPolylist::ZFXPolylist(void) {
   m_pPolys = NULL;
   m_Num    = 0;
   m_Max    = 0;
   } // constructor
/*----------------------------------------------------------------*/


/**
 * Destructor
 */
ZFXPolylist::~ZFXPolylist(void) {
   if (m_pPolys) {
      free(m_pPolys);
      m_pPolys = NULL;
      }
   } // destructor
/*----------------------------------------------------------------*/


/**
 * adds a new polygon to the existing list
 */
bool ZFXPolylist::AddPolygon(const ZFXPolygon &Poly) {
   // adjust memory first
   if ( !CheckMem() ) return false;

   // add polygon
   m_pPolys[m_Num].CopyOf( Poly );
   m_Num++;
   return true;
   } // AddPolygon
/*----------------------------------------------------------------*/


/**
 * reset the whole list
 */
void ZFXPolylist::Reset(void) {
   if (m_pPolys) {
      free(m_pPolys);
      m_pPolys = NULL;
      }
   m_Num = 0;
   m_Max = 0;
   } // Reset
/*----------------------------------------------------------------*/


/**
 * check or enlarge the available dynamic memory
 */
bool ZFXPolylist::CheckMem(void) {
   if (m_Num < m_Max) return true;

   m_Max += 100;

   int nSize = sizeof(ZFXPolygon)*m_Max;
   m_pPolys = (ZFXPolygon*)realloc(m_pPolys, nSize);

   memset(&m_pPolys[m_Num], 0, sizeof(ZFXPolygon)*100);
   return (m_pPolys != NULL);
   } // Reset
/*----------------------------------------------------------------*/


