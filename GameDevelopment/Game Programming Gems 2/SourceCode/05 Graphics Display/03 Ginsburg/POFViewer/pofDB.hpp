/* Copyright (C) Dan Ginsburg ang Dave Gosselin, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Ginsburg ang Dave Gosselin, 2001"
 */
//  pofDB.hpp  - Version 0.1
//
//    Evan Hart - ATI Research - 5/11/2000
//
//   This is the header file used to define constants,
//  macros, and functions used with the Portable Object Format.
//
///////////////////////////////////////////////////////////////

#ifndef POFDB_HPP
#define POFDB_HPP

#include "pofDB.h"

//
// 2 dimensional map from names/instances to
// objects
////////////////////////////////////////////////

//
//
////////////////////////////////////////////////
template <class T>
pofMap2D<T>::pofMap2D(){
  m_objects=NULL;
  m_iCount=NULL;
  m_instances=NULL;
  m_names=NULL;
  m_nCount=0;
}

//
//
////////////////////////////////////////////////
template <class T>
pofMap2D<T>::~pofMap2D(){
  int ii,gg;

  for (ii=0;ii<m_nCount;ii++){
    for (gg=0;gg<m_iCount[ii];gg++){
      delete m_objects[ii][gg];
    }
    delete []m_objects[ii];
    delete []m_instances[ii];
  }
  
  if (m_objects)
    delete []m_objects;
  
  if (m_instances)
    delete []m_instances;

  if (m_names)
    delete []m_names;

  if (m_iCount)
    delete []m_iCount;
}

//
//
////////////////////////////////////////////////
template <class T>
const T* pofMap2D<T>::GetObject(unsigned int name, unsigned int instance){
  int ii,gg;

  //loop through the list of names
  for (ii=0; ii<m_nCount; ii++){
    //check to see if the name is found
    if (name==m_names[ii]){
      //loop through instances
      for (gg=0; gg<m_iCount[ii]; gg++){
        //if instance is found return
        if (m_instances[ii][gg]==instance)
          return m_objects[ii][gg];
      }
    }
  }

  return NULL;
}

//
//
////////////////////////////////////////////////
template <class T>
void pofMap2D<T>::AddObject(unsigned int name, unsigned int instance, T* obj){
  int ii;
  int *tempi;
  unsigned int *tempui;
  unsigned int **temppui;
  T   ***temppT;
  T   **tempT;

  for (ii=0; ii<m_nCount; ii++){
    if (m_names[ii]==name)
      break;
  }

  if (ii==m_nCount){
    //alloc a new name
    if (!(m_nCount%5)){
      //names
      tempui=new unsigned int[m_nCount+5];
      memcpy(tempui,m_names,sizeof(int)*m_nCount);
      delete []m_names;
      m_names=tempui;
      //instance counts
      tempi=new int[m_nCount+5];
      memcpy(tempi,m_iCount,sizeof(int)*m_nCount);
      delete []m_iCount;
      m_iCount=tempi;
      //instance names
      temppui=new unsigned int *[m_nCount+5];
      memcpy(temppui,m_instances,sizeof(unsigned int *)*m_nCount);
      delete []m_instances;
      m_instances=temppui;
      //objects
      temppT=new T**[m_nCount+5];
      memcpy(temppT,m_objects,sizeof(T **)*m_nCount);
      delete []m_objects;
      m_objects=temppT;
    }
    m_instances[m_nCount]=new unsigned int[5];
    m_objects[ii]=new T*[5];
    m_iCount[m_nCount]=1;
    m_instances[m_nCount][0]=instance;
    m_names[m_nCount]=name;
    m_objects[ii][0]=obj;
    m_nCount++;
  }
  else {
    //alloc a new instance
    if (!(m_iCount[ii]%5)){
      //instance names
      tempui=new unsigned int[m_iCount[ii]+5];
      memcpy(tempui,m_instances[ii],sizeof(unsigned int)*m_iCount[ii]);
      delete []m_instances[ii];
      m_instances[ii]=tempui;
      //objects
      tempT=new T*[m_iCount[ii]+5];
      memcpy(tempT,m_objects[ii],sizeof(T *)*m_iCount[ii]);
      delete []m_objects[ii];
      m_objects[ii]=tempT;
    }
    m_instances[ii][m_iCount[ii]]=instance;
    m_objects[ii][m_iCount[ii]]=obj;
    m_iCount[ii]++;
  }
}

//
//
////////////////////////////////////////////////
template <class T>
const unsigned int* pofMap2D<T>::GetNames(){
  return m_names;
}

//
//
////////////////////////////////////////////////
template <class T>
const unsigned int* pofMap2D<T>::GetInstances(unsigned int name){
  int ii;

  //loop through names
  for (ii=0; ii<m_nCount; ii++){
    //is the name found?
    if (m_names[ii]==name)
      return m_instances[ii];
  }

  return NULL;
}

//
//
////////////////////////////////////////////////
template <class T>
int pofMap2D<T>::GetNumNames(){
  return m_nCount;
}

//
//
////////////////////////////////////////////////
template <class T>
int pofMap2D<T>::GetNumInstances(unsigned int name){
  int ii;

  //loop through names
  for (ii=0; ii<m_nCount; ii++){
    //is the name found?
    if (m_names[ii]==name)
      return m_iCount[ii];
  }

  return 0;
}

//
// 1 dimensional map from names to objects
////////////////////////////////////////////////


template <class T>
pofMap1D<T>::pofMap1D(){
  m_objects=NULL;
  m_names=NULL;
  m_count=0;
}

template <class T>
pofMap1D<T>::~pofMap1D(){
  int ii;

  for (ii=0; ii<m_count; ii++){
    if (m_objects[ii])
      delete m_objects[ii];
  }

  if (m_objects)
    delete []m_objects;

  if (m_names)
    delete []m_names;
}

template <class T>
const T* pofMap1D<T>::GetObject(unsigned int name){
  int ii;

  for (ii=0; ii<m_count; ii++){
    if (m_names[ii]==name)
      return m_objects[ii];
  }
  return NULL;
}

template <class T>
void pofMap1D<T>::AddObject(unsigned int name, T* obj){
  unsigned int *tempui;
  T **tempT;

  if (!(m_count%5)){
    //names
    tempui=new unsigned int[m_count+5];
    memcpy(tempui,m_names,sizeof(unsigned int)*m_count);
    delete []m_names;
    m_names=tempui;
    //objects
    tempT=new T*[m_count+5];
    memcpy(tempT,m_objects,sizeof(T *)*m_count);
    delete []m_objects;
    m_objects=tempT;
  }

  m_names[m_count]=name;
  m_objects[m_count]=obj;
  m_count++;
}

template <class T>
const unsigned int* pofMap1D<T>::GetNames(){
  return m_names;
}

template <class T>
int pofMap1D<T>::GetNumObjects(){
  return m_count;
}

#endif

