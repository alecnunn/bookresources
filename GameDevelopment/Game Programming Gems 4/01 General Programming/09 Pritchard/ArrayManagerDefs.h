// ============================================================================
// ArrayManagerDefs.h
//
// provides the CArrayItem helper class
// ============================================================================


#ifndef _ARRAYMANAGERDEFS_H_
#define _ARRAYMANAGERDEFS_H_


// ============================================================================
//
// CArrayItem structure
//
// This struct is just a sample structure to illustrate usage with the
// CArrayManager and CAddressSpaceArrayManager classes. 
//
// In this example we just made it an array of chars to fill up 1K
// with a single element.  In actual usage, you would replace CArrayItem
// with your own data structre(s) or templatize it to generate a version
// of the the Array Manager for several different structures.
//
// ============================================================================


class CArrayItem
{
   public:
   char    m_Data[1024];
} ;


// ============================================================================

#endif
