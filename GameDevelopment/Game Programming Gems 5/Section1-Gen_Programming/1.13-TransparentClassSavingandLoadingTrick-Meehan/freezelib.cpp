#include <stdio.h>
#include <stdlib.h>
#include "freezelib.h"

using namespace std;
using namespace FreezeLib;
using namespace FreezeLibImp;

/********************************//********************************/
// Freezable
/********************************//********************************/

/********************************/
// Check to see if the type is valid.  If not, attempt to match it to a type ID using the
// name provided by the user.  This will fail (in an assert()) if the user forgot to
// register the type.
u32 Freezable::AffirmType ( void ) {

    if ( mTypeID == INVALID_TYPE )
        mTypeID = FreezeMgr::Get ()->FindTypeIDByName ( GetClassName ());

    assert ( mTypeID != INVALID_TYPE );
    return mTypeID;
}

/********************************/
// Add the Freezable to the FreezeMgr only if it was just constructed by a ConcreteCreator
// and only if the FreezeMgr isn't thawing.
Freezable::Freezable ( void ) {

    FreezeMgr* freezeMgr = FreezeMgr::Get ();

    // If the FreezeMgr is running a thaw test, tag this Freezable to be excluded from
    // the thaw if it doesn't match the Freezable whose constructor was just called.
    if ( freezeMgr->IsTestingThaw ()) {
        if ( !(( freezeMgr->mThawTestFreezable == this ) && (   freezeMgr->mThawTestType == mTypeID ))) {
            // This Freezable must be nested in another Freezable.
            // That means its constructor is called automatically.
            // So... tag it to be excluded from the thaw.
              FreezableInfo* info = freezeMgr->FindFreezableInfo ( this );
              if ( info ) info->mNeedsThaw = false;
        }
    }
    else if ( freezeMgr->IsConstructing ()) {
    
        mTypeID = INVALID_TYPE;
        
        // Assume that this Freezable needs a thaw unless proven otherwise...
        freezeMgr->AddFreezable ( this, true );
    }
}

/********************************//********************************/
// MemBlock
/********************************//********************************/

/********************************/
// Adds info for a Freezable to the MemBlock.  The address of the Freezable
// must fall within the memory range represented by the MemBlock.
void MemBlock::AddFreezable ( Freezable* freezable, bool needsThaw ) {
    
    assert ( ContainsAddress ( freezable ));
    FreezableInfo& freezableInfo = mFreezableInfoMap [ freezable ];
    freezableInfo.mFreezable = freezable;
    freezableInfo.mNeedsThaw = needsThaw;
    freezableInfo.mBlockOffset = GetLocalOffset ( freezable );
}

/********************************/
// Adds info for a pointer.
void MemBlock::AddPtr ( void* addr ) {

    assert ( ContainsAddress ( addr ));
    PtrInfo& ptrInfo = mPtrInfoMap [ addr ];
    ptrInfo.mPtrAddr = addr;
    ptrInfo.mBlockOffset = GetLocalOffset ( addr );
}

/********************************/
// Check to see if the address in question falls within the memory range
// represented by the MemBlock.
bool MemBlock::ContainsAddress ( void* addr ) {

    if ((( u32 )addr >= ( u32 )mMem ) && (( u32 )addr < (( u32 )mMem + mSize ))) return true;
    return false;
}

/********************************/
FreezableInfo* MemBlock::FindFreezableInfo ( Freezable* freezable ) {
    FreezableInfoMapIt freezableInfoMapIt = mFreezableInfoMap.find  ( freezable );
    if ( freezableInfoMapIt == mFreezableInfoMap.end ()) return 0;
    return &freezableInfoMapIt->second;
}

/********************************/
// Given a pointer in memory, figure out its location relative to the base
// address of the MemBlock.
u32 MemBlock::GetLocalOffset ( void* addr ) {
    assert ( ContainsAddress ( addr ));
    return (( u32 )addr - ( u32 )mMem );
}

/********************************/
MemBlock::MemBlock ( void ) :
    mMem ( 0 ),
    mCreator ( 0 ),
    mInFreezeQueue ( false ),
    mIsFreezable ( true ) {
}

/********************************/
// This is an indirectly recursive routine that adds the MemBlock in question
// to the list of MemBlocks to write during the freeze and follows any pointers
// out of the MemBlock to other MemBlocks.
u32 MemBlock::FreezeRecursive ( void ) {

    // Well, it had better be!
    assert ( mIsFreezable );

    // If the MemBlock has already been remapped and queued, bail
    if ( mInFreezeQueue ) return mBaseOffset;

    FreezeMgr* freezeMgr = FreezeMgr::Get ();
    
    // Add self to the list of MemBlocks to freeze
    freezeMgr->mMemBlockFreezeList.push_back ( this );
    mInFreezeQueue = true;
    
    // Determine this block's base offset into the file
    mBaseOffset = freezeMgr->AllocVirtualBlock ( mSize );

    // It's safe to remap these now that we know the base offset
    // of the MemBlock.
    RemapFreezables ();

    // This is the function that actually performs the recurse,
    // as each pointer must be followed to the MemBlock that contains
    // it in order to determine its correct offset.
    RemapPtrs ();

    return mBaseOffset;
}

/********************************/
// The destructors of classes within the MemBlock are not called in this implementation
// because records of each class are not restored after a thaw; there's no way to know
// what classes are in a MemBlock.  We could use the list of Freezables (as in the comment
// below) but this would be arbitrary: Freezables are only restored in edit mode.
// I would suggest instead that you add your own mechanism for destructing only certain
// Freezables.  In my own implementation I never invoke any Freezable's destructor.  I
// instead call a virtual cleanup routine on Freezables explicitely registered by the
// end user (and also for all Freezables with serialized data).
MemBlock::~MemBlock ( void ) {

    FreezeMgr* freezeMgr = FreezeMgr::Get ();

    /*
    // Destruct Freezables only
    FreezableInfoMapIt freezableInfoMapIt = mFreezableInfoMap.begin ();
    for ( ; freezableInfoMapIt != mFreezableInfoMap.end (); ++freezableInfoMapIt ) {
        Freezable* freezable = freezableInfoMapIt->second.mFreezable;
        assert ( freezable );
        u32 typeID = freezable->AffirmType ();
        freezeMgr->mCreatorVec [ typeID ]->Destruct ( freezable );
    }
    */

    if ( mMem ) freezeMgr->OSFree ( mMem );
}

/********************************/
void MemBlock::RemapFreezables ( void ) {

    FreezeMgr* freezeMgr = FreezeMgr::Get ();

    // Remap and list all the Freezables and tag them for restoration
    FreezableInfoMapIt freezableInfoMapIt = mFreezableInfoMap.begin ();
    for ( ; freezableInfoMapIt != mFreezableInfoMap.end (); ++freezableInfoMapIt ) {
    
        FreezableInfo& freezableInfo = freezableInfoMapIt->second;
        assert ( freezableInfo.mFreezable );

        freezeMgr->mFreezableFreezeList.push_back ( &freezableInfo );
        freezableInfo.mFileOffset = mBaseOffset + freezableInfo.mBlockOffset;
    }
}

/********************************/
// In this implementation, even the NULL pointers are added to the file's pointer
// table.  We do this to make the implementation of 'edit' mode easier.  If edit mode
// is not important to you then by all means, exclude NULL pointers from the table!
void MemBlock::RemapPtrs ( void ) {

    FreezeMgr* freezeMgr = FreezeMgr::Get ();

    // Remap and list all the pointers and tag them for restoration
    PtrInfoMapIt ptrInfoMapIt = mPtrInfoMap.begin ();
    for ( ; ptrInfoMapIt != mPtrInfoMap.end (); ++ptrInfoMapIt ) {
    
        PtrInfo& ptrInfo = ptrInfoMapIt->second;
        void* ptr = ptrInfo.GetPtr (); // Get the contents of the pointer

        // Get the location of the pointer in the packed file
        ptrInfo.mFileOffset = mBaseOffset + ptrInfo.mBlockOffset;
        
        if ( ptr ) {
            // Follow the pointer to whatever other MemBlock it points to
            // This is part of the recursive mechanism of the flood fill
            ptrInfo.mFileOffsetPtr = freezeMgr->FreezeMemBlock ( ptr );
        }
        else ptrInfo.mFileOffsetPtr = PtrInfo::NULL_OFFSET;
        
        // Tag the pointer to be written out to the file
        freezeMgr->mPtrInfoFreezeList.push_back ( &ptrInfo );
    }
}

/********************************/
// Easy, just dump the memory to the file.
void MemBlock::Write ( FILE* file ) {

    FreezeMgr* freezeMgr = FreezeMgr::Get ();
    
    // Write out the mem block.  We'll patch the pointers later.
    fwrite ( mMem, mSize, 1, file );
    
    // It's OK to do this here, since the MemBlock will be removed from the queue
    // shortly after this
    mInFreezeQueue = false;
}

/********************************//********************************/
// FreezeMgr
/********************************//********************************/

/********************************/
// Find the MemBlock that contains the address of the Freezable and add info for
// that Freezable to the MemBlock.
void FreezeMgr::AddFreezable ( Freezable* freezable, bool needsThaw ) {

    MemBlock* memBlock = FindMemBlock ( freezable );
    if ( memBlock ) memBlock->AddFreezable ( freezable, needsThaw );
}

/********************************/
// Create a MemBlock to allocate and track a memory range constructed by the creator.
// This *should* be the only user-controlled method that can construct a class to be frozen.
FreezeLibImp::MemBlock* FreezeMgr::AddMemBlock ( AbstractCreator* creator, u32 nElements ) {

    // Don't call anything that will add a MemBlock (like AllocTyped ()) from inside a
    // a constructor!  That will make a huge mess of things!
    assert ( mFreezeMgrState != STATE_CONSTRUCTING );
    u32 prevState = mFreezeMgrState;
    mFreezeMgrState = STATE_CONSTRUCTING;

    u32 elemSize;
    u32 size;
    u32 total;
    
    if ( creator ) {
    
        // Don't try to create anything during a thaw (i.e. from inside of a Freezable
        // constructor).
        assert ( !IsThawing ());
    
        elemSize = creator->GetClassSize ();
        size = elemSize * nElements;
        total = nElements;
    }
    else {
        elemSize = nElements;
        size = nElements;
        total = 1;
    }
    
    assert ( size );

    void* mem = OSAlloc ( size );
    assert ( mem );

    MemBlock* memBlock = &mMemBlockMap [ mem ]; 
    memBlock->mMem = mem;
    memBlock->mSize = size;
    memBlock->mElementSize = elemSize;
    memBlock->mTotalElements = total;
    
    if ( creator )
        creator->Construct ( mem, total );

    mFreezeMgrState = prevState;
    return memBlock;
}

/********************************/
// Find the MemBlock that contains the address of the pointer and add info for
// that pointer to the MemBlock.
void FreezeMgr::AddPtr ( void* addr ) {

    MemBlock* memBlock = FindMemBlock ( addr );
    if ( memBlock ) memBlock->AddPtr ( addr );
}

/********************************/
// Iterate through all the Freezables in the current list of classes to be frozen
// and affirm their types.
void FreezeMgr::AffirmTypes ( void ) {

    FreezableFreezeListIt freezableFreezeListIt = mFreezableFreezeList.begin ();
    for ( ; freezableFreezeListIt != mFreezableFreezeList.end (); ++freezableFreezeListIt ) {
        FreezableInfo* freezableInfo = *freezableFreezeListIt;
        freezableInfo->mFreezable->AffirmType ();
    }
}

/********************************/
void* FreezeMgr::AllocRaw ( u32 size ) {

    assert ( !IsThawing ());

    assert ( size );
    void* mem = OSAlloc ( size );
    assert ( mem );

    MemBlock* memBlock = &mMemBlockMap [ mem ]; 
    memBlock->mMem = mem;
    memBlock->mSize = size;
    memBlock->mElementSize = 1;
    memBlock->mTotalElements = 1;
    
    return mem; 
}

/********************************/
// Pretty dumb function.  Just gloms the size onto the virtual block and returns
// the new top.
u32 FreezeMgr::AllocVirtualBlock ( u32 size ) {

    u32 top = mVirtualBufferTop;
    mVirtualBufferTop += size;
    return top;
}

/********************************/
FreezableInfo* FreezeMgr::FindFreezableInfo ( Freezable* freezable ) {

    MemBlock* memBlock = FindMemBlock ( freezable );
    if ( memBlock ) return memBlock->FindFreezableInfo ( freezable );
    return 0;
}

/********************************/
// Search through the map of MemBlocks (indexed by the base addresses of the memory ranges
// they represent) and find which MemBlock (if any) contains the range that the address
// falls in.
MemBlock* FreezeMgr::FindMemBlock ( void* addr ) {

    // No MemBlocks, so the address can't possibly be in a monitored range.
    if ( !mMemBlockMap.size ()) return 0;

    // Find the first MemBlock with a range address greated than addr.
    MemBlockMapIt memBlockMapIt = mMemBlockMap.upper_bound ( addr );
    
    // If it's the first MemBlock, the addr is not in any range (since the
    // MemBlocks are sorted by address).
    if ( memBlockMapIt == mMemBlockMap.begin ()) return 0;
    
    // The previous MemBlock must have a base address less than or equal to the addr.
    --memBlockMapIt;
    
    // See if the MemBlock actually does contain addr.  If so, we found it!
    MemBlock* memBlock = &memBlockMapIt->second;
    if ( memBlock->ContainsAddress ( addr )) return memBlock;

    // The addr is somewhere in memory between two separate MemBlocks.  So... it
    // isn't in a MemBlock.  Return NULL.
    return 0;
}

/********************************/
// Maps user supplied class names onto indices in the FreezeMgr's table of
// AbstractCreator pointers.
u32 FreezeMgr::FindTypeIDByName ( char const* name ) {

    ClassNameMapIt classNameMapIt = mClassNameMap.find ( name );
    if ( classNameMapIt == mClassNameMap.end ()) return Freezable::INVALID_TYPE;
    return classNameMapIt->second;
}

/********************************/
// Releases all the MemBlocks tracked by the FreezeMgr and flushes the lists of
// objects being frozen (which really should be empty at this stage anyway...)
void FreezeMgr::Flush ( void ) {

    mMemBlockMap.clear ();
    mMemBlockFreezeList.clear ();
    mPtrInfoFreezeList.clear ();
    mFreezableFreezeList.clear ();
}

/********************************/
void FreezeMgr::Free ( void* mem ) {

    assert ( !IsFreezingNow ());
    if ( !mMemBlockMap.erase ( mem ))
        OSFree ( mem );
}

/********************************/
void FreezeMgr::Freeze ( const char* filename, void* addr ) {

    if ( !addr ) return;
    FILE* file = fopen ( filename, "wb" );

    MemBlock* memBlock = FindMemBlock ( addr );
    assert ( memBlock );
    
    // Make sure we're freezing from the base of a block.  Why?
    // Because when we thaw, the address returned will always be the
    // base of the first block packed.  If the user is passing us the
    // address of a nested class or something and expects to get
    // that address back, they'll be dissapointed.  Of course we could
    // store the offset at the head of the file to handle that case...
    assert ( memBlock->mMem == addr );
    
    // Reset the virtual buffer
    mVirtualBufferTop = 0;

    // This will determine the MemBlock's location in the file and recursively repeat
    // the process on other blocks by following pointers.
    memBlock->FreezeRecursive ();
    
    // We affirm the type identifiers of the Freezables (if any) here (instead of
    // in the Freezable's constructor) so that we only have to affirm Freezables about
    // to be saved.  We have to do this after Remap() so that the list of Freezables
    // will exist and before WriteMemBlocks() since the Freezables are part of the
    // data packed into the file.
    AffirmTypes ();
    
    // Now we test the thaw.  This routine calls the constructor for each Freezable
    // in faux thaw mode.  Any nested Freezables whose constructors would be called
    // automatically (by another Freezable being thawed) are tagged for exclusion
    // from the thaw.
    // You might wonder why we can't just do this test in the Freezable's constructor.
    // Try it that way and you'll understand when you look carefully at the case
    // of multiple Freezables nested inside of a non-freezable.
    TestThaw ();

    // Write out the size of the packed data
    fwrite ( &mVirtualBufferTop, sizeof ( u32 ), 1, file );
    
    // Write the stuff & things
    WriteMemBlocks ( file );
    WritePtrInfos ( file );
    WriteFreezables ( file );
    WriteSerialData ( file );
    
    // Done, so clear the lists
    mMemBlockFreezeList.clear ();
    mPtrInfoFreezeList.clear ();
    mFreezableFreezeList.clear ();
    
    fclose ( file );
}

/********************************/
u32 FreezeMgr::FreezeMemBlock ( void* addr ) {

    // Do nothing if passed a null address
    if ( !addr ) return 0;

    // Search for the MemBlock that contains the address
    MemBlock* memBlock = FindMemBlock ( addr );
    assert ( memBlock );
    
    // We need to return the remapped offset of the address passed in
    u32 ptrOffset = ( u32 )addr - ( u32 )memBlock->mMem;
    return memBlock->FreezeRecursive () + ptrOffset;
}

/********************************/
FreezeMgr::FreezeMgr ( void ) :
    mThawMem ( 0 ),
    mThawTestFreezable ( 0 ),
    mThawTestType ( Freezable::INVALID_TYPE ),
    mFreezeMgrState ( STATE_IDLE ) {
}

/********************************/
FreezeMgr::~FreezeMgr ( void ) {
}

/********************************/
// A cheesy singleton...
FreezeMgr* FreezeMgr::Get ( void ) {
    static FreezeMgr single;
    return &single;
}

/********************************/
// There are pending objects waiting to be written out to the file.
bool FreezeMgr::IsFreezingNow ( void ) {

    if (    mMemBlockFreezeList.size () ||
            mPtrInfoFreezeList.size () ||
            mFreezableFreezeList.size ()) return true;
    return false;
}

/********************************/
bool FreezeMgr::IsConstructing ( void ) {
    return ( mFreezeMgrState == STATE_CONSTRUCTING );
}

/********************************/
bool FreezeMgr::IsTestingThaw ( void ) {
    return ( mFreezeMgrState == STATE_TESTING );
}

/********************************/
bool FreezeMgr::IsThawing ( void ) {
    return ( mFreezeMgrState == STATE_THAWING );
}

/********************************/
// Stupid wrapper for malloc ().
void* FreezeMgr::OSAlloc ( u32 size ) {
    return malloc ( size );
}

/********************************/
// Stupid wrapper for free ().
void FreezeMgr::OSFree ( void* mem ) {
    free ( mem );
}

/********************************/
// Scans the table of Freezables and uses the ID of each Freezable to index
// the vector of AbstractCreators and construct the class.
void FreezeMgr::ReadFreezables ( FILE* file, bool edit ) {
    
    // Read in offsets to all the classes that need to be thawed and thaw them
    while ( true )  {
    
        // This is the offset of the next Freezable in the list.
        u32 offset;
        fread ( &offset, sizeof ( u32 ), 1, file );
        
        // If it's NULL, we're done.
        if ( offset == PtrInfo::NULL_OFFSET ) break;
        
        // Remap the offset into the heap, where    the file is being unpacked to.
        Freezable* freezable = ( Freezable* )( offset + ( u32 )mThawMem );
        
        // Make sure the Freezable is legal.
        assert ( freezable->mTypeID != Freezable::INVALID_TYPE );
        
        // Use the factory to construct the Freezable class in place, thereby
        // restoring its virtual function table pointer.
        mCreatorVec [ freezable->mTypeID ]->Construct ( freezable, 1 );

        // If we're in edit mode, add info from the Freezable in anticipation of another
        // freeze later.
        if ( edit ) AddFreezable ( freezable, true );
    }
}

/********************************/
// The name is a misnomer, since only one memory block is read.  This is the block
// create in-file by packing all the other blocks together.  The FreezeMgr will
// only create an actual MemBlock to track the range if we're thawing in 'edit' mode.
void FreezeMgr::ReadMemBlocks ( FILE* file, bool edit ) {

    // Read in the mem block size
    u32 size;
    fread ( &size, sizeof ( u32 ), 1, file );

    // Allocate the mem block
    MemBlock* memBlock = AddMemBlock ( 0, size );
    
    if ( !edit ) memBlock->mIsFreezable = false;
    
    // Grab the address of the contiguous block
    mThawMem = memBlock->mMem;
    assert ( mThawMem );

    // Read in the mem block
    fread ( mThawMem, size, 1, file );
}

/********************************/
// This is the workhorse that scans the table of pointer remaps and does the dirty work
// of patching them back into the heap.  Since we're supprting 'edit' mode, we have to
// read NULL pointers too so that they may be added to the list of pointers to track.
void FreezeMgr::ReadPtrInfos ( FILE* file, bool edit ) {

    // Read in the number of pointers to thaw
    u32 totalPtrInfos;
    fread ( &totalPtrInfos, sizeof ( u32 ), 1, file );
    
    // Read in offsets to all the pointers that need to be thawed and thaw them
    for ( u32 count = 0; count < totalPtrInfos; ++count ) {
        
        // Read in the address of the pointer in question.  The address is relative
        // to the base of the packed data block.
        void** ptrAddr;
        fread ( &ptrAddr, sizeof ( u32 ), 1, file );
        
        // Remap the address of the pointer to be relative to the data block in the heap.
        ptrAddr = ( void** )(( u32 )ptrAddr + ( u32 )mThawMem );
        
        // Now read in the contents of the pointer, as they were before the freeze (again,
        // a relative offset).
        void* ptrValue;
        fread ( &ptrValue, sizeof ( u32 ), 1, file );

        // We only have to remap the pointer if it was not NULL.
        if (( u32 )ptrValue != PtrInfo::NULL_OFFSET )
            ( *ptrAddr ) = ( void* )(( u32 )ptrValue + ( u32 )mThawMem );
        
        // If we're in 'edit' mode, create an info record for the pointer.  We have to
        // do this even if it was NULL because one of the main reasons for 'edit' mode
        // to exists, presumably, is so that the user may augment their game data by
        // filling pointers that were previously NULL.
        if ( edit ) AddPtr ( ptrAddr );
    }
}

/********************************/
// Visit each Freezable that wrote serial data during the freeze and allow it to
// read that serial data back in.  The Freezable must advance the file pointer by the
// exact number of bytes it wrote out during the thaw or we'll assert().
void FreezeMgr::ReadSerialData ( FILE* file, bool edit  ) {

    // De-serialize any serialized data.
    // We'll loop until we hit a NULL offset.
    while ( true ) {
    
        // This is the offset of the next Freezable in the list.
        u32 offset;
        fread ( &offset, sizeof ( u32 ), 1, file );
        
        // If it's NULL, we're done.
        if ( offset == PtrInfo::NULL_OFFSET ) break;
        
        // Remap the offset into the heap.
        Freezable* freezable = ( Freezable* )( offset + ( u32 )mThawMem );

        // The number of byte written during the freeze by SerializeOut().
        u32 totalBytes;
        fread ( &totalBytes, sizeof ( u32 ), 1, file );
    
        // Grab the current position of the file cursor.
        long int serialStart = ftell ( file );
        
        // Give the Freezable a chance to read in its serial data.
        freezable->SerializeIn ( file );
        
        // Grab the current position of the file cursor again.
        long int serialEnd = ftell ( file );
    
        // Make sure that the Freezable didn't do something stupid like rolling
        // the file cursor back below the start of its serial data.
        assert ( serialEnd >= serialStart );
        
        // Make sure the number of bytes read in by SerializeIn() matches the original
        // number of bytes written out.
        assert ( totalBytes == ( serialEnd - serialStart ));
    }
}

/********************************/
// Well, why not?
void FreezeMgr::ReserveTypeTable ( u32 size ) {

    mCreatorVec.reserve ( size );
}

/********************************/
// This is a stupidly slow method of weeding out redundant constructor calls.  Basically, if
// we have a Freezable nested inside another Freezable, we only need to call the constructor for
// the containing class.  C++ will handle the rest.
// To find out which Freezables are nested (w/o asking the user for extra data) we use the brute
// force method of calling each Freezable's constructor and seeing which other constructors are
// called automatically.  The owners of those other constructors should then tag themselves to
// be excluded from the thaw.
// This has poor worst case performance, so think of a better solution.
void FreezeMgr::TestThaw ( void ) {

    u32 prevState = mFreezeMgrState;
    mFreezeMgrState = STATE_TESTING;

    // Write out offsets to all the classes that need to be thawed
    FreezableFreezeListIt freezableFreezeListIt = mFreezableFreezeList.begin ();
    for ( ; freezableFreezeListIt != mFreezableFreezeList.end (); ++freezableFreezeListIt ) {

        FreezableInfo* freezableInfo = *freezableFreezeListIt;
        
        // Only test the Freezables that would be included in the thaw.
        if ( freezableInfo->mNeedsThaw ) {
        
            Freezable* freezable = freezableInfo->mFreezable;
            
            // Make sure the Freezable is legal.
            assert ( freezable->mTypeID != Freezable::INVALID_TYPE );
            
            // This is the freezable being tested...
            // So let the Freezable's constructor know to do nothing.
            mThawTestFreezable = freezable;
            mThawTestType = freezable->mTypeID;
            
            // Call the constructor in faux thaw mode to do some stuff
            mCreatorVec [ freezable->mTypeID ]->Construct ( freezable, 1 );
        }
    }

    mFreezeMgrState = prevState;
}

/********************************/
void* FreezeMgr::Thaw ( char const* filename, bool edit ) {

    void* mem;

    FILE* file = fopen ( filename, "rb" );

    u32 prevState = mFreezeMgrState;
    mFreezeMgrState = STATE_THAWING;
    
    ReadMemBlocks ( file, edit );
    ReadPtrInfos ( file, edit );
    ReadFreezables ( file, edit );
    ReadSerialData ( file, edit );

    mFreezeMgrState = prevState;

    fclose ( file );
    
    mem = mThawMem;
    mThawMem = 0;
    return mem;
}

/********************************/
// Write the offsets of all the Freezables included in the current freeze.
void FreezeMgr::WriteFreezables ( FILE* file ) {

    // Write out offsets to all the classes that need to be thawed
    FreezableFreezeListIt freezableFreezeListIt = mFreezableFreezeList.begin ();
    for ( ; freezableFreezeListIt != mFreezableFreezeList.end (); ++freezableFreezeListIt ) {

        FreezableInfo* freezableInfo = *freezableFreezeListIt;
        if ( !freezableInfo->mNeedsThaw ) continue;
        u32 fileOffset = freezableInfo->mFileOffset;
        fwrite ( &fileOffset, sizeof ( u32 ), 1, file );
    }

    // Write out a NULL terminator
    // We're out of Freezables, so terminate the list by writing a NULL offset.
    u32 nada = PtrInfo::NULL_OFFSET;
    fwrite ( &nada, sizeof ( u32 ), 1, file );
}

/********************************/
// Iterate through all the MemBlocks and pack their memory into the file's contiguous block.
void FreezeMgr::WriteMemBlocks ( FILE* file ) {
    
    // Write out offsets to all the classes that need to be thawed
    MemBlockFreezeListIt memBlockFreezeListIt = mMemBlockFreezeList.begin ();
    for ( ; memBlockFreezeListIt != mMemBlockFreezeList.end (); ++memBlockFreezeListIt ) {
    
        MemBlock* memBlock = *memBlockFreezeListIt;
        memBlock->Write ( file );
    }
}

/********************************/
// Write out the offset to each pointer, and the contents of each pointer (also as a relative
// offset).
void FreezeMgr::WritePtrInfos ( FILE* file ) {

    // Write out the number of pointers to restore
    u32 totalPtrInfos = ( u32 )( mPtrInfoFreezeList.size ());
    fwrite ( &totalPtrInfos, sizeof ( u32 ), 1, file );
    
    // Write out offsets to all the pointers that need to be thawed
    PtrInfoFreezeListIt ptrInfoFreezeListIt = mPtrInfoFreezeList.begin ();
    for ( ; ptrInfoFreezeListIt != mPtrInfoFreezeList.end (); ++ptrInfoFreezeListIt ) {
    
        PtrInfo* ptrInfo = *ptrInfoFreezeListIt;
    
        u32 fileOffset = ptrInfo->mFileOffset;
        fwrite ( &fileOffset, sizeof ( u32 ), 1, file );

        u32 normalized = ptrInfo->mFileOffsetPtr;
        fwrite ( &normalized, sizeof ( u32 ), 1, file );
    }
}

/********************************/
// Iterate through the list of Freezables and call SerializeOut() on each one.  Only
// the Freezables that actually write something will be included in the list to call
// SerializeIn() during the thaw.
void FreezeMgr::WriteSerialData ( FILE* file ) {

    // Iterate through the list of Freezables.
    FreezableFreezeListIt freezableFreezeListIt = mFreezableFreezeList.begin ();
    for ( ; freezableFreezeListIt != mFreezableFreezeList.end (); ++freezableFreezeListIt ) {
    
        FreezableInfo* freezableInfo = *freezableFreezeListIt;
        assert ( freezableInfo->mFreezable );
        Freezable* freezable = freezableInfo->mFreezable;
    
        // It's true: 0 byes have been written by this Freezable so far.
        u32 totalBytes = 0; 
        
        // Grab the current location of the file cursor.  This is the base address
        // of the serial data record.
        long int serialHeader = ftell ( file );
        
        // Now write the offset to the Freezable relative to the base of the contiguous
        // block in the file.
        u32 fileOffset = freezableInfo->mFileOffset;
        fwrite ( &fileOffset, sizeof ( u32 ), 1, file ); // Offset to file (currently NULL)
        
        // Get the location of the file cursor right before the place where we'll write the
        // size of the serial data.  We need this so we'll know where to seek back to later, after
        // we actually know the size of the serial data.
        long int serialSize = ftell ( file );
        
        // Write out a placeholder for the size of the serial data.  We'll seek back and patch this later.
        fwrite ( &totalBytes, sizeof ( u32 ), 1, file ); // Total bytes written during serialization

        // Grab the file cursor location before any serial data is written.
        long int serialStart = ftell ( file );
        
        // Ask the Freezable to write its serial data.
        freezable->SerializeOut ( file );
        
        // Grab the location of the file cursor after the data was written (in any).
        long int serialEnd = ftell ( file );

        // Make sure the user didn't roll the file cursor backwards.
        assert ( serialEnd >= serialStart );
        
        // Figure out the total number of bytes written.
        totalBytes = serialEnd - serialStart;

        if ( !totalBytes ) {
        // Nothing was serialized, so just seek back to the very start, even before the
        // Freezable offset was written.
            fseek ( file, serialHeader, SEEK_SET );
            
        }
        else {
        // Something was serialized, so write out the total bytes and seek to the next data block
        // after patching the data size value.
        
            // Seek back to where we put the placeholder for the size.
            fseek ( file, serialSize, SEEK_SET );
            
            // Write the actual size of the serial data.
            fwrite ( &totalBytes, sizeof ( u32 ), 1, file );
            
            // See forward to the end of the serial data, ready for the next Freezable.
            fseek ( file, serialEnd, SEEK_SET );
        }
    }

    // We're out of Freezables, so terminate the list by writing a NULL offset.
    u32 nada = PtrInfo::NULL_OFFSET;
    fwrite ( &nada, sizeof ( u32 ), 1, file );
}
