#ifndef FREEZELIB_H
#define FREEZELIB_H

#include <stdio.h>
#include <assert.h>
#include <new>

#include <vector>
#include <string>
#include <list>
#include <map>

/********************************/
namespace FreezeLib {

    typedef unsigned long u32;
    class Freezable;
    class FreezeMgr;

	/********************************/
    namespace FreezeLibImp {

        /********************************//********************************/
        // Creator 
        /********************************//********************************/

        // These classes are used to implement the abstract factory functionality of the
        // FreezeMgr.  The FreezeMgr keeps a vector of AbstractCreator pointers, each of
        // which points at a ConcreteCreator for a specific class.

        /********************************/
        // I am including Destruct() as a method of this creator for example only.  This
        // implementation of the FreezeMgr does not use it.
        class AbstractCreator {
        public:
            virtual void Construct ( void* mem, u32 nElements = 1 ) = 0;
            virtual void Destruct ( void* mem, u32 nElements = 1 ) = 0;
            virtual u32 GetClassSize ( void ) = 0;
        };

        /********************************/
        // This template class is a time saver.  It creates a concrete creator for TYPE.
        template < typename TYPE >
        class ConcreteCreator :
            public AbstractCreator {
            public:

            /********************************/
            // I used loops instead of the array form of placement new because
            // the array form requires extra memory to store the size of the array.
            void Construct ( void* mem, u32 nElements ) {
                assert ( nElements );
                for ( u32 i = 0; i < nElements; ++i )
                    new ( &((( TYPE* )mem )[ i ])) TYPE (); // The empty () is just a formality.
            };
            
            /********************************/
            // Included just for kicks, so you can see the explicit call to the
            // destructor.  There it is.  See it?
            void Destruct ( void* mem, u32 nElements ) {
                assert ( nElements );
                for ( u32 i = 0; i < nElements; ++i )
                    (( TYPE* )mem )[ i ].TYPE::~TYPE (); // It's here!
            };
            
            /********************************/
            u32 GetClassSize ( void ) {
                return sizeof ( TYPE );
            };
        };

        /********************************//********************************/
        // PtrInfo 
        /********************************//********************************/

        /********************************/
        // This struct is used to keep track of a single instance of a FreezePtr.  Because
        // the memory footprint of each FreezePtr must be no larger than a regular pointer,
        // we have to keep extra info in a separate class.  You could probably derive everything
        // here as the pointer is being remapped, thereby removing the need for PtrInfo, BUT
        // if you want to add special features later (like being able to set a pointer to an
        // enternal file) you will be glad this exists.
        struct PtrInfo {

            // Since pointers are mapped into relative offset space during a freeze, NULL
            // (a pointer set to zero) loses its meaning.  We instead map NULL pointers to
            // this value.
            enum {
                NULL_OFFSET = 0xffffffff,
            };
            
            // These members track the location of the pointer itself
            void* mPtrAddr; // Location of the pointer in the heap
            u32 mBlockOffset; // Pointer offset from base of containing mem block
            u32 mFileOffset; // Pointer offset from base of file (only valid during a freeze)
            
            // This member is used to remap the *contents* of the pointer during a freeze
            u32 mFileOffsetPtr; // The file offset of whatever the thing is pointing at
            
            /********************************/
            // This returns the value of the freese pointer tracked by this class.
            // Since mPtrAddr is the address of the pointer in question, we retrieve the value
            // of that pointer by casting mPtrAddr to a pointer-to-pointer and the dereferencing it.
            void* PtrInfo::GetPtr ( void ) {
                assert ( mPtrAddr );
                return *(( void** )mPtrAddr );
            };
        };

        /********************************//********************************/
        // FreezableInfo 
        /********************************//********************************/

        /********************************/
        // A lightweight version of PtrInfo.  Probably could have made it the base
        // for that struct... but why?  Anyway, if you decide to pimp out Freezable
        // with some extra bells and whistles, you'll want this separate struct.
        struct FreezableInfo {
            
            bool mNeedsThaw;
            
            Freezable* mFreezable; // Location in the heap
            u32 mBlockOffset; // Pointer offset from base of parent class
            u32 mFileOffset; // Pointer offset from base of file
            
            FreezableInfo ( void ) : mNeedsThaw ( false ) {};
        };

        /********************************//********************************/
        // MemBlock 
        /********************************//********************************/

        /********************************/
        // This class remembers everything about anything that gets allocated by
        // the FreezeMgr, including its type (if the type is registered) and its size.
        class MemBlock {
            friend class FreezeLib::FreezeMgr;
            
            // Used to find out whether or not the block is included in the current freeze
            bool mInFreezeQueue;

            // True by default.  Freeze() will choke if this is false.
            bool mIsFreezable;

            AbstractCreator* mCreator; // Not valid after a thaw

            void* mMem; // Location in the heap
            u32 mBaseOffset; // Location in the file; valid only during a freeze

            u32 mSize; // Total size of memory pointed to by mMem
            u32 mElementSize; // Total size of each element in mMem (same as mSize if only 1 element)
            u32 mTotalElements; // Total elements (always 1 unless mMem is tracking an array)
            
            // List of pointers to be remapped
            typedef std::map < void*, PtrInfo >::iterator PtrInfoMapIt;
            std::map < void*, PtrInfo > mPtrInfoMap;
            
            // List of all Freezables
            typedef std::map < Freezable*, FreezableInfo >::iterator FreezableInfoMapIt;
            std::map < Freezable*, FreezableInfo > mFreezableInfoMap;

            void AddFreezable ( Freezable* freezable, bool needsThaw );
            void AddPtr ( void* ptrAddr );
            bool ContainsAddress ( void* addr );
            FreezableInfo* FindFreezableInfo ( Freezable* freezable );
            u32 FreezeRecursive ( void );
            u32 GetLocalOffset ( void* addr );
            void RemapFreezables ( void );
            void RemapPtrs ( void );
            void Write ( FILE* file );
            
        public:
            MemBlock ( void );
            ~MemBlock ( void );
        };

    }; // namespace FreezeLibImp;

    /********************************//********************************/
    // FreezeMgr
    /********************************//********************************/

    /********************************/
    // This singleton acts as an abstract factory.  It also tracks memory allocated
    // by the user to be frozen later.
    class FreezeMgr {

        friend class FreezeLibImp::MemBlock;
        friend class FreezePtrBase;
        friend class Freezable;

        enum {
            // The FreezeMgr is doing nothing special.
            STATE_IDLE,
            
            // A Freezable is being explicitely constructed by the user.
            STATE_CONSTRUCTING,
            
            // The FreezeMgr is testing for nested Freezables.
            STATE_TESTING,
            
            // The FreezeMgr is performing a thaw of some sort.
            STATE_THAWING,
        };

        u32 mFreezeMgrState;

        // This is used to test the freezables for nestedness. (I don't think that's a word.)
        // Only the root Freezable being constructed can match both the address and the type
        // because C++ doesn't allow nested classes of the same type.
        // Actually, only the type ID is really needed to perform this test.
        Freezable* mThawTestFreezable;
        u32 mThawTestType;

        /********************************//********************************/
        // Abstract factory stuff

        // Table of constructor classes
        typedef std::vector < FreezeLibImp::AbstractCreator* >::iterator CreatorVecIt;
        std::vector < FreezeLibImp::AbstractCreator* > mCreatorVec;

        // Table to map type names onto ID numbers
        typedef std::map < std::string, u32 >::iterator ClassNameMapIt;
        std::map < std::string, u32 > mClassNameMap;

        /********************************//********************************/
        // Mem block tracking stuff

        // Map of addresses onto memory blocks (allocated by the FreezeMgr)
        typedef std::map < void*, FreezeLibImp::MemBlock >::iterator MemBlockMapIt;
        std::map < void*, FreezeLibImp::MemBlock > mMemBlockMap;

        /********************************//********************************/
        // Freeze process variables and lists

        // This is used to compute the size of the packed memory.
        // At the start of a freeze, it is set to 0.  As each MemBlock is tagged
        // for inclusion in the freeze, the size of the block is added to this.  This is
        // how the offset of each block from the base of the file is determined.
        u32 mVirtualBufferTop;

        // Pointer to the memory block currently being thawed (if any).
        // Used to remap pointers and such.
        // This is set back to NULL after the thaw.
        void* mThawMem;

        // The following lists track elements that need to be packed into the file
        // being created during a thaw.  They are fully cleared after each thaw.

        // List of MemBlocks in *current* freezing only
        typedef std::list < FreezeLibImp::MemBlock* >::iterator MemBlockFreezeListIt;
        std::list < FreezeLibImp::MemBlock* > mMemBlockFreezeList;

        // List of pointers in *current* freezing only
        typedef std::list < FreezeLibImp::PtrInfo* >::iterator PtrInfoFreezeListIt;
        std::list < FreezeLibImp::PtrInfo* > mPtrInfoFreezeList;

        // List of Freezables in *current* freezing only
        // Please don't laugh at the stupid name
        typedef std::list < FreezeLibImp::FreezableInfo* >::iterator FreezableFreezeListIt;
        std::list < FreezeLibImp::FreezableInfo* > mFreezableFreezeList;

        void AddFreezable ( Freezable* freezable, bool needsThaw );
        FreezeLibImp::MemBlock* AddMemBlock ( FreezeLibImp::AbstractCreator* creator, u32 nElements );
        void AddPtr ( void* ptrAddr );
        void AffirmTypes ( void );
        u32 AllocVirtualBlock ( u32 size );
        FreezeLibImp::FreezableInfo* FindFreezableInfo ( Freezable* freezable );
        FreezeLibImp::MemBlock* FindMemBlock ( void* addr );
        u32 FindTypeIDByName ( char const* name );
        u32 FreezeMemBlock ( void* addr );
        bool IsFreezingNow ( void );
        void* OSAlloc ( u32 size );
        void OSFree ( void* mem );
        void ReadFreezables ( FILE* file, bool edit );
        void ReadMemBlocks ( FILE* file, bool edit );
        void ReadPtrInfos ( FILE* file, bool edit );
        void ReadSerialData ( FILE* file, bool edit );
        void TestThaw ( void );
        void WriteFreezables ( FILE* file );
        void WriteMemBlocks ( FILE* file );
        void WritePtrInfos ( FILE* file );
        void WriteSerialData ( FILE* file );

    public:

        void* AllocRaw ( u32 size );
        void Flush ( void );
        void Free ( void* mem );
        void Freeze ( char const* filename, void* addr );
        FreezeMgr ( void );
        ~FreezeMgr ( void );
        static FreezeMgr* Get ( void );
        bool IsConstructing ( void );
        bool IsTestingThaw ( void );
        bool IsThawing ( void );
        void ReserveTypeTable ( u32 size );
        void* Thaw ( char const* filename, bool edit = false );

        /********************************/
        template < typename TYPE >
        TYPE* AllocTyped ( u32 nElements = 1 ) {
            FreezeLibImp::ConcreteCreator < TYPE > creator;
            return ( TYPE* )( AddMemBlock ( &creator, nElements )->mMem );
        };

        /********************************/
        // This is a helper function that declares a ConcreteCreator for the Freezable and
        // adds a pointer to it to the FreezeMgr's table of creators.
        // In this case, I'm partial to using a static, but of course you can rework it to use new.
        template < typename TYPE >
        void RegisterType ( void ) {
            TYPE temp;
            static FreezeLibImp::ConcreteCreator < TYPE > creator;
            Freezable* freezable = &temp; // Type mismatch error if TYPE not derived from Freezable
            mClassNameMap [ freezable->GetClassName () ] = mCreatorVec.size ();
            mCreatorVec.push_back ( &creator );
        };
        
        /********************************/
        template < typename TYPE >
        TYPE* ThawTyped ( const char* filename, bool edit = false ) {
            return ( TYPE* )Thaw ( filename, edit );
        };
    };

    /********************************//********************************/
    // FreezePtr
    /********************************//********************************/

    /********************************/
    // Holds the pointer and does the basic work of adding it to the FreezeMgr
    // upon construction.  The pointer will not be added if the FreezeMgr is currently
    // thawing (since the only constructors called are being called as part of the thaw).
    // The pointer will also not be added if it is not inside a memory range tracked by
    // a MemBlock.  Note that there is a bit of overhead associated with searching for
    // a MemBlock that contains the pointer.
    class FreezePtrBase {
    protected:
    
        void* mPtr;
        
        /********************************/
        FreezePtrBase ( void ) {
            FreezeMgr* freezeMgr = FreezeMgr::Get ();
            if ( !freezeMgr->IsThawing ()) {
                mPtr = 0;
                freezeMgr->AddPtr ( &mPtr );
            }
        };
    };

    /********************************/
    // Bare minimum operator overloads as inlines.  I didn't do this for my
    // own implementation, 'cause I'm not a huge fan of operator overloads.
    template < typename TYPE >
    class FreezePtr :
        public FreezePtrBase {
        public:

        /********************************/
        inline void operator = ( TYPE* assign ) {
            mPtr = assign;
        };

        /********************************/
        inline TYPE* operator-> ( void ) const {
            return ( TYPE* )mPtr;
        };

        /********************************/
        inline TYPE& operator*() const {
            return *( TYPE* )mPtr;
        };

        /********************************/
        inline operator TYPE* ( void ) {
            return ( TYPE* )mPtr;
        };
    };

    /********************************//********************************/
    // Freezable 
    /********************************//********************************/

    /********************************/
    // You only have to inherit this class if you want to use virtual functions or
    // serialized data.  Be sure to overload GetClassName() to return a unique string
    // const.  Including the namespace hierarchy (if any) is recommended.
    class Freezable {
        friend class FreezeLibImp::MemBlock;
        friend class FreezeMgr;
        
        enum {
            INVALID_TYPE = 0xffffffff,
        };

        u32 mTypeID;
        u32 AffirmType ( void );
    
    public:
        Freezable ( void );
        virtual ~Freezable ( void ) {};
        virtual char const* GetClassName ( void ) const = 0;
        
        // WARNING: The FreezeMgr *does not* call any destructors.  If you are going
        // to allocate memory you must implement a mechanism to track and free it!
        // In my own implementaion, I track all classes that use SerializeIn() and call
        // a virtual cleanup routine when the mem block containing them is freed.
        virtual void SerializeIn ( FILE* binFile ) {};
        virtual void SerializeOut ( FILE* binFile ) {};
    };

} // namespace FreezeLib;

#endif
