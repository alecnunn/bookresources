/* Copyright (C) Peter Dalton, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Peter Dalton, 2001"
 */

bool InitializeMemoryManager() {
  static bool hasBeenInitialized = false;
  if (s_manager)               return true;
  else if (hasBeenInitialized) return false;
  else {
    s_manager = (MemoryManager*)malloc(sizeof(MemoryManager));
    s_manager->intialize();
    atexit( releaseMemoryManager );
    hasBeenIntialized = true;
    return true;
  }
}
void releaseMemoryManager() {
  NumAllocations = s_manager->m_numAllocations;
  s_manager->release();    // Releases the hash table and calls 
  free( s_manager );       //  the dumpLogReport() method
  s_manager = NULL;
}

