/* Copyright (C) Dan Ginsburg ang Dave Gosselin, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Ginsburg ang Dave Gosselin, 2001"
 */
//  pofDB.h  - Version 0.1
//
//    Evan Hart - ATI Research - 5/22/2000
//
//   This is the header file used to define constants,
//  macros, and functions used with the Portable Object Format.
//
///////////////////////////////////////////////////////////////

#ifndef POFDB_H
#define POFDB_H

/*******************************************************\
 *                                                     *
 * The following are templates that store a mapping    *
 * from 1 or 2 dimensional name-space to a set of      *
 * objects. They are presently implemented with a      *
 * simple, inefficient linear search. Future revisions *
 * should replace the search with a better algorithm   *
 *                                                     *
\*******************************************************/

//
// 2 dimensional map from names/instances to
// objects
////////////////////////////////////////////////
template <class T>
class pofMap2D{
  private:
    T ***m_objects;
    unsigned int *m_names;
    unsigned int **m_instances;

    int m_nCount;
    int *m_iCount;

  public:
    pofMap2D();
    ~pofMap2D();

    const T* GetObject(unsigned int name, unsigned int instance);
    void AddObject(unsigned int name, unsigned int instance, T* obj);
    const unsigned int* GetNames();
    const unsigned int* GetInstances(unsigned int name);
    int GetNumNames();
    int GetNumInstances(unsigned int name);
};

//
// 1 dimensional map from names to objects
////////////////////////////////////////////////
template <class T>
class pofMap1D{
  private:
    T **m_objects;
    unsigned int *m_names;

    int m_count;

  public:
    pofMap1D();
    ~pofMap1D();

    const T* GetObject(unsigned int name);
    void AddObject(unsigned int name, T* obj);
    const unsigned int* GetNames();
    int GetNumObjects();
};

#endif