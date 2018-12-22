//--------------------------------------------------------------------------
// Author: Natasha Tatarchuk
//--------------------------------------------------------------------------

#ifndef OBJECT_MANAGER_H
#define OBJECT_MANAGER_H

#include <map>
#include <Typeinfo.h>

#include "Destroyer.h"


// String comparison function for the map used in the object manager:

struct StringLessThan
{
    bool operator()(const char* s1, const char* s2) const
    {
        return ( strcmp( s1, s2 ) < 0 );
    }
};

//=============================================================================
// Default class for an entry into the object manager. 
//
// IMPORTANT NOTE:
// Whatever data type will be assigned to this object instance during its FIRST data assignment, 
// that data type will persist as this object's data type for the duration of its
// existence. There is no way to change an object's data type once it's been assigned
// some data.
//=============================================================================
class ObjectContainer
{

public:

    //-------------------------------------------------------------------------
    ObjectContainer() 
    {
        m_pData      = NULL;
        m_pDestroyer = NULL;
        m_pTypeName  = NULL;
    };

    //-------------------------------------------------------------------------
    ~ObjectContainer () 
    {
        // Delete the destroyer object which will delete the data stored in this object instance:
        if ( m_pDestroyer )
        {
            delete m_pDestroyer;
            m_pDestroyer = NULL;
        }
    };

    //-------------------------------------------------------------------------
    // Return type id of this object instance:
    //-------------------------------------------------------------------------
    type_info* GetTypeID ()
    {
        return m_pTypeName;
    }

    //-------------------------------------------------------------------------
    // Checks whether this object instance is empty:
    //-------------------------------------------------------------------------
    bool IsEmpty ()
    {
        if ( m_pData )
            return false;
        else
            return true;
    }

    //-------------------------------------------------------------------------
    // ObjectContainer::checkType ()
    // 
    // Check whether the given type is the same as the object's data type. 
    //
    // Returns: true if the type is the same, false if otherwise.
    //-------------------------------------------------------------------------
    template <class DataType> bool CheckType ( DataType& refValue )
    {
        if ( (*m_pTypeName) == typeid ( DataType ) )
            return true;
        else
            return false;

    }   // End of ObjectContainer::checkType ()

    //-------------------------------------------------------------------------
    // Retrieve data from this object instance in the appropriate data type (DataType). Will return false
    // if either the type given is not the same as the type of this particular object instance or otherwise
    // failed:
    //-------------------------------------------------------------------------
    template <class DataType> bool GetData ( DataType& refData )
    {
        // Check whether the data type is the same as this object entry's type:
        if ( (*m_pTypeName) != typeid ( DataType ) )
        {
            return false;
        }

        // Cast the data stored into the appropriate type:
        DataType* pTmpData = static_cast <DataType *> ( m_pData );

        // Retrieve the data value:
        refData = (*pTmpData);

        return true;

    }   // End of ObjectContainer::getData (..)

    //-------------------------------------------------------------------------
    // Retrieve data from this object in the appropriate data type (DataType). 
    // Note that this version does not check whether the data type of the
    // parameter is the same as the data type of the data value stored in this
    // object, and will attempt to cast it to the appropriate value. Use at your own
    // risk:
    //-------------------------------------------------------------------------
    template <class DataType> bool GetDataNotTypeSafe ( DataType& refData )
    {
        // Cast the data stored into the given type:
        DataType* pTmpData = static_cast <DataType*> ( m_pData );

        // Retrieve the data value:
        refData = (*pTmpData);

        return true;
    
    }   // End of ObjectContainer::getDataNotTypeSafe (..)
    
    //-------------------------------------------------------------------------
    // Assign some data to this object. Note that the type info will also be assigned. If any 
    // other data was stored previously in this object, it will be deleted. See the comment on 
    // top of the class declaration about data assignment for some precautions.
    template <class DataType> bool SetData ( DataType const &dataValue )
    {
        // First make sure to delete any data previously stored in this object:
        if ( m_pData )
        {
           delete( m_pDestroyer );
           m_pDestroyer = NULL;
        }

        // Create new storage for the data value:
        DataType* pData = new DataType ( dataValue );

        m_pData = pData;

        // Save the type information for this object:
        m_pTypeName = const_cast <type_info *> ( &( typeid ( DataType )) );

        // Create a destroyer for the data for this data object:
        m_pDestroyer = new Destroyer<DataType> ( pData );

        return true;
    }

private: 
    void*       m_pData;         // Pointer to the data held in this object instance
    IDestroyer* m_pDestroyer;    // Pointer to destruction factory, class that will clear out our data
    type_info*  m_pTypeName;     // Type name saved off so we can do run-time type check

};  // End of class ObjectContainer declaration/definition


//=============================================================================
typedef std::map< char const*, ObjectContainer*, StringLessThan > ObjectMap;

//=============================================================================
// ObjectManager class. 
// 
// Created to hold arbitrary-typed objects using object names in strings as keys
// into the actual object manager. Order of storage is not important. It's fairly
// easy to customize the object manager to use a different object identifier, such
// as GUID or any other game-time ID to safely distinguish the objects.
// 
//=============================================================================
class ObjectManager  
{
public:

   //-------------------------------------------------------------------------
   ObjectManager() {};
   
   //-------------------------------------------------------------------------
   virtual ~ObjectManager()
   {        
      // Iterate through the object manager deleting its object instances:
      ObjectMap::iterator it = m_manager.begin ();
      
      while( it != m_manager.end() )
      {
         // Delete the data for the object instance:
         delete ( (*it).second );
         
         // Remove this object instance from the manager:
         m_manager.erase ( it++ );
      }
   }
   
   //-------------------------------------------------------------------------
   // Queries the object manager for an object instance of the specified type with checking whether this
   // type is the same as the object instance's native type.
   //
   // Input parameters: 
   //      strObjectName: string name for the given object instance
   //      refResult:     storage for the result found. 
   //      ObjectType:    object type
   //
   // Returns: true is object instance is found, false if failed to find this particular object instance.
   //
   // Notes: will fail if the object instance stored under this key in the dictionary is of a different type
   //        than the one requested. 
   //
   //-------------------------------------------------------------------------
   template <class ObjectType> bool GetObject ( char const *strObjectName, 
                                                ObjectType &refResult ) const
   {
      // Try to locate the object in the object manager first:
      ObjectMap::const_iterator it = m_manager.find ( strObjectName );
      
      // Check whether the element was found or have we reached the end of the manager storage:
      if ( it == m_manager.end() )
      {
         // Failed to find it!
         return false;
      }
      
      // First check if the object instance is empty (not containing any data):
      if ( (*it).second->IsEmpty () )
      {
         // The object instance is empty: fail:
         return false;
      }
      
      // Then check that the found object type is the same as the requested type for this object:
      if ( ! (*it).second->CheckType ( refResult ) )
      {
         // ERROR:
         return false;
      }
      
      // Retrieve the actual object data from this object entry:
      (*it).second->GetData ( refResult );
      
      return true;
      
   }
   
   //-------------------------------------------------------------------------
   // Query the manager for an object entry of the specified type (not type-safe)
   //
   // Input parameters: 
   //      strObjectName: string name for the given object instance
   //      refResult:     storage for the result found. 
   //      ObjectType:     object data type
   //
   // Returns: true is object is found, false if failed to find this particular object.
   //
   // Notes: Will not fail if the given type ObjectType is not the same as the 
   //        selected object instance's native type, instead will attemt to cast into
   //        ObjectType. However, this may create potential memory leaks and
   //        overall not a safe procedure. Use w/ caution.
   //
   //-------------------------------------------------------------------------
   template <class ObjectType> bool GetObjectNotTypeSafe ( char const *strObjectName, ObjectType &refResult ) const
   {
      // Try to find this object in the object manager:
      ObjectMap::const_iterator it = m_manager.find ( strObjectName );
      
      // Check whether the object instance was found or have we reached the end of the manager storage:
      if ( it == m_manager.end() )
      {
         // Failed to find it!
         return false;
      }
      
      // First check if the object instance doesn't contain any object data:
      if ( (*it).second->IsEmpty () )
      {
         // The object instance is empty:
         return false;
      }
      
      // Retrieve the data from this object instance:
      (*it).second->GetDataNotTypeSafe ( refResult );
      
      return true;
      
   }
   
   //-------------------------------------------------------------------------
   // Retrieves the data type id of a given object entry. Note that if the object entry  
   // doesn't exist in the object manager or the entry's object data hasn't been set, this 
   // method will return NULL. 
   //
   //-------------------------------------------------------------------------
   type_info* GetObjectEntryType ( char const * strObjectName )
   {
      // Try to find this object entry in the object manager:
      ObjectMap::const_iterator it = m_manager.find ( strObjectName );
      
      // Check whether the object entry instance was found or have we reached the end of the object manager storage:
      if ( it == m_manager.end () )
      {
         // Failed to find it:
         return NULL;
      }
      
      // Return this object instance entry's type id:
      return (*it).second->GetTypeID ();
      
   }
   
   //-------------------------------------------------------------------------
   // Check whether the given type is the same as the object entry's object data type. 
   //
   // Returns: true if the type is the same, false if otherwise or if
   //          the object entry was not found in the object manager.
   //-------------------------------------------------------------------------  
   template <class ObjectType> bool CheckObjectEntryType ( char const* strObjectEntryName, ObjectType& refValue )
   {
      // Try to find this object entry in the object manager:
      ObjectMap::const_iterator it = m_manager.find ( strObjectEntryName );
      
      // Check whether the element was found or have we reached the end of the object manager's storage:
      if ( it == m_manager.end () )
      {
         // Failed to find it:
         return false;
      }
      
      // Check this object entry's data type given the type of the value sent:
      return (*it).second->CheckType ( refValue );
      
   }
   
   
   //-------------------------------------------------------------------------
   // Allows setting object data for a particular objectentry 
   // 
   // Input parameters:
   //  strObjectName: name of the object entry in question
   //  refValue:      reference to the object data value we're trying to assign to the object entry
   //  ObjectType:    the object data type for the given object entry
   //
   // Notes:
   //  - This method cannot add a new object entry to the dictionary. That must be done w/ the 
   //    AddObjectEntry call.
   //  - The very first setting of data value for a particular object entry sets is data type for
   //    the duration of the object entry's life span. That data type cannot be changed.
   //  - Will fail if the object entry is non-empty and ObjectType is different from the data type of
   //    the given object entry
   //
   // Returns: false upon failure, true upon success.
   //
   //-------------------------------------------------------------------------
   template <class ObjectType> bool SetObjectEntryData ( char const *strObjectName, ObjectType const &refValue )
   {
      // Try to locate the object entry instance with the specified name in the object manager:
      ObjectMap::const_iterator it = m_manager.find ( strObjectName );
      
      // Check whether the object entry was found or have we reached the end of the object manager's storage:
      if ( it == m_manager.end() )
      {
         // Failed to find the object instance!
         return false;
      }
      
      // Check that the found object entry type is the same as the requested type for this object entry:
      if ( ! (*it).second->CheckType ( refValue ) )
      {
         // ERROR:
         return false;
      }
      
      // Try to set the data value for this found object entry:
      if ( ! (*it).second->SetData ( refValue ) )
      {
         // The data type is invalid for the given object entry, error:
         return false;
      }
      
      return true;
      
   }
   
   
   //-------------------------------------------------------------------------
   // Adds a new object` entry to the object manager given its name as the identifier and sets its value. 
   // Will check whether there are already object entries with the given name in which case 
   // it will return false and not add a new object to the object manager.
   //
   //-------------------------------------------------------------------------
   template <class ObjectType> bool AddObjectEntry ( char const *strObjectName, ObjectType const &refValue )
   {
      // First try to locate an entry with the specified name in the object manager:
      ObjectMap::iterator it = m_manager.find ( strObjectName );
      
      // If another object is found in the object manager with the same name, cannot proceed with
      // adding a new object entry, exit:
      if ( ! (it == m_manager.end() ))
      {
         return false;
      }
      
      // The object manager doesn't contain an object entry with the given name, now let's allocate
      // a new object entry for the name specified:
      ObjectContainer* pNewObjectEntry = new ObjectContainer();
      m_manager [ strObjectName ] = pNewObjectEntry;
      
      // Assign object data to the new object entry:
      pNewObjectEntry->SetData ( refValue );
      
      return true;
   }
   
   //-------------------------------------------------------------------------
   // Removes an object entry from the object manager given its name. Will check whether such object entry
   // exists in the object manager. If the object entry doesn't exist in the object manager, this
   // method will do absolutely nothing. Note that this method doesn't actually delete object data that is
   // stored in the object entry.
   //-------------------------------------------------------------------------
   void RemoveObjectEntry ( char const *strObjectName )
   {        
      // Try to find this object entry in the object manager:
      ObjectMap::iterator it = m_manager.find ( strObjectName );
      
      // Check whether the element was found or have we reached the end of the object manager:
      if ( it == m_manager.end() )
      {
         // This object entry is not in the object manager, don't need to do anything
         return;
      }
      
      // Remove the found object manager from the dictionary:
      m_manager.erase ( it );  
   }   

   //-------------------------------------------------------------------------
   // Deletes an object entry from the object manager given its name. Will check whether such object entry
   // exists in the object manager. If the object entry doesn't exist in the object manager, this
   // method will do absolutely nothing. Note that this method DOES actually delete object data that is
   // stored in the object entry.
   //-------------------------------------------------------------------------
   void DeleteObjectEntry ( char const *strObjectName )
   {        
      // Try to find this object entry in the object manager:
      ObjectMap::iterator it = m_manager.find ( strObjectName );
      
      // Check whether the element was found or have we reached the end of the object manager:
      if ( it == m_manager.end() )
      {
         // This object entry is not in the object manager, don't need to do anything
         return;
      }
      
      // Delete object data for the object entry:
      delete ( (*it).second );

      // Remove the found object manager from the dictionary:
      m_manager.erase ( it );  
   }   

   
private:
   
   // Object manager storage containing all object entries:
   ObjectMap m_manager;      
   
};  // End of class ObjectManager declaration


#endif 