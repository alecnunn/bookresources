#include <new>
#include <stdio.h>
#include <stdlib.h>
#include "freezelib.h"

using namespace std;
using namespace FreezeLib;

/********************************/
// A helper function for the sample
void ReadString ( FILE* file, char* str, u32 sizeLim ) {

    u32 size;
    fread ( &size, sizeof ( u32 ), 1, file );
    assert ( size < sizeLim );
    if ( size ) fread ( str, size, 1, file );
}

/********************************/
// A helper function for the sample
void WriteString ( FILE* file, char* str ) {

    u32 size = strlen ( str ) + 1;
    fwrite ( &size, sizeof ( u32 ), 1, file );
    if ( size ) fwrite ( str, size, 1, file );
}

/********************************/
// Inherit Freezable to enable virtual functions and serial data.
class Printer : public Freezable {
public:
    
    // Use a FreezePtr to point to the next element in the list.
    FreezePtr < Printer > mNext;
    
    // The virtual function to test each element.
    virtual void Print ( void ) = 0;
    
    /********************************/
    // Just iterate through the list and test each element.
    void PrintList ( void ) {
        for ( Printer* printer = this; printer; printer = printer->mNext )
            printer->Print ();
        printf ( "\n" );
    };

    /********************************/  
    Printer ( void ) {
    
        // Only initialize members if the FreezeMgr is not performing a thaw.
        if ( !FreezeMgr::Get ()->IsThawing ())
            mNext = 0;
    };

    /********************************/
    // Test the data (if any) written by SerializeOut ()  
    void SerializeIn ( FILE* file ) {
        char str [ 256 ];
        ReadString ( file, str, 256 );
        printf ( "%s\n", str );
    };
};

/********************************/
class Banana : public Printer {
public:

    /********************************/
    Banana ( void ) {
        FreezeMgr* freezeMgr = FreezeMgr::Get ();
        if ( freezeMgr->IsConstructing ())
            printf ( "Constructing a Banana\n" );
        else if ( freezeMgr->IsThawing () && !freezeMgr->IsTestingThaw ())
            printf ( "Thawing a Banana\n" );
    };
    void Print ( void ) { printf ( "Banana::Print()\n" ); };
    void SerializeOut ( FILE* file ) { WriteString ( file, "Serialized data for Banana." ); };
    char const* GetClassName ( void ) const { return "Banana"; };
};

/********************************/
class Papaya : public Printer {
public:

    /********************************/
    Papaya ( void ) {
        FreezeMgr* freezeMgr = FreezeMgr::Get ();
        if ( freezeMgr->IsConstructing ())
            printf ( "Constructing a Papaya\n" );
        else if ( freezeMgr->IsThawing () && !freezeMgr->IsTestingThaw ())
            printf ( "Thawing a Papaya\n" );
    };
    void Print ( void ) { printf ( "Papaya::Print()\n" ); };    
    void SerializeOut ( FILE* file ) { WriteString ( file, "Serialized data for Papaya." ); };
    char const* GetClassName ( void ) const { return "Papaya"; };
};

/********************************/
class Blueberry : public Printer {
public:

    /********************************/
    Blueberry ( void ) {
        FreezeMgr* freezeMgr = FreezeMgr::Get ();
        if ( freezeMgr->IsConstructing ())
            printf ( "Constructing a Blueberry\n" );
        else if ( freezeMgr->IsThawing () && !freezeMgr->IsTestingThaw ())
            printf ( "Thawing a Blueberry\n" );
    };
    void Print ( void ) { printf ( "Blueberry::Print()\n" ); };
    void SerializeOut ( FILE* file ) { WriteString ( file, "Serialized data for Blueberry." ); };
    char const* GetClassName ( void ) const { return "Blueberry"; };
};

/********************************/
class Mango : public Printer {
public:

    /********************************/
    Mango ( void ) {
        FreezeMgr* freezeMgr = FreezeMgr::Get ();
        if ( freezeMgr->IsConstructing ())
            printf ( "Constructing a Mango\n" );
        else if ( freezeMgr->IsThawing () && !freezeMgr->IsTestingThaw ())
            printf ( "Thawing a Mango\n" );
    };
    void Print ( void ) { printf ( "Mango::Print()\n" ); };
    void SerializeOut ( FILE* file ) { WriteString ( file, "Serialized data for Mango." ); };
    char const* GetClassName ( void ) const { return "Mango"; };
};

/********************************/
class TripleMango : public Printer {
public:

    Mango mMangoArray [ 3 ];

    /********************************/
    TripleMango ( void ) {
        FreezeMgr* freezeMgr = FreezeMgr::Get ();
        if ( freezeMgr->IsConstructing ())
            printf ( "Constructing a TripleMango\n" );
        else if ( freezeMgr->IsThawing () && !freezeMgr->IsTestingThaw ())
            printf ( "Thawing a TripleMango\n" );
    };
    void Print ( void ) { printf ( "TripleMango::Print()\n" ); };
    void SerializeOut ( FILE* file ) { WriteString ( file, "Serialized data for TripleMango." ); };
    char const* GetClassName ( void ) const { return "TripleMango"; };
};

/********************************/
class FruitBowl {
public:

    Banana mBanana;
    Papaya mPapaya;
    Blueberry mBlueberry;
    TripleMango mTripleMango;
    
    FreezePtr < Printer > mPrinterPtrArray [ 4 ];
    
    /********************************/
    FruitBowl ( void ) {
        FreezeMgr* freezeMgr = FreezeMgr::Get ();
        if ( freezeMgr->IsConstructing ()) {
            printf ( "Constructing a FruitBowl\n" );
            
            mPrinterPtrArray [ 0 ] = &mBanana;
            mPrinterPtrArray [ 1 ] = &mPapaya;
            mPrinterPtrArray [ 2 ] = &mBlueberry;
            mPrinterPtrArray [ 3 ] = &mTripleMango;
        }
        else if ( freezeMgr->IsThawing () && !freezeMgr->IsTestingThaw ())
            printf ( "Thawing a FruitBowl\n" );
    };
    
    /********************************/
    void Print ( void ) {
        printf ( "Fruit Bowl::Print()\n" );
        for ( u32 i = 0; i < 4; ++i )
            mPrinterPtrArray [ i ]->Print ();
        printf ( "\n" );
    };
};

/********************************/
void RegisterTypes ( void ) {

    printf ( "REGISTERING TYPES.\n" );

    FreezeMgr* freezeMgr = FreezeMgr::Get ();

    freezeMgr->RegisterType < Banana > ();
    freezeMgr->RegisterType < Papaya > ();
    freezeMgr->RegisterType < Blueberry > ();
    freezeMgr->RegisterType < Mango > ();
    freezeMgr->RegisterType < TripleMango > ();
    
    printf ( "\n" );
}

/********************************/
void MakeFruitListFile ( void ) {

    printf ( "MAKING FRUIT LIST.\n" );

    FreezeMgr* freezeMgr = FreezeMgr::Get ();

    Printer* p1   = ( Printer* )freezeMgr->AllocTyped < Banana >();
    Printer* p2   = ( Printer* )freezeMgr->AllocTyped < Papaya >();
    Printer* p3   = ( Printer* )freezeMgr->AllocTyped < Blueberry >();

    p1->mNext =   p2;
    p2->mNext =   p3;
    p3->mNext =   0;

    freezeMgr->Freeze ( "fruitList.ice", p1 );
    freezeMgr->Flush ();
    
    printf ( "\n" );
}

/********************************/
void AppendTripleMango ( void ) {

    printf ( "APPENDING TRIPLE MANGO (IN EDIT MODE).\n" );

    FreezeMgr* freezeMgr = FreezeMgr::Get ();

    Printer* p1 = ( Printer* )freezeMgr->AllocTyped < TripleMango >();
    p1->mNext = freezeMgr->ThawTyped < Printer >( "fruitList.ice", true );

    freezeMgr->Freeze ( "fruitList.ice", p1 );
    freezeMgr->Flush ();
    
    printf ( "\n" );
}

/********************************/
void TestFruitListFile ( void ) {

    printf ( "TESTING PRINTER CLASS FILE.\n" );

    FreezeMgr* freezeMgr = FreezeMgr::Get ();

    Printer* file1 = freezeMgr->ThawTyped < Printer >( "fruitList.ice" );
    file1->PrintList ();
    
    freezeMgr->Free ( file1 );
}

/********************************/
void MakeFruitBowlFile ( void ) {

    printf ( "MAKING FRUIT BOWL.\n" );

    FreezeMgr* freezeMgr = FreezeMgr::Get ();

    FruitBowl* fb = ( FruitBowl* )freezeMgr->AllocTyped < FruitBowl >();

    freezeMgr->Freeze ( "fruitBowl.ice", fb );
    freezeMgr->Flush ();
    
    printf ( "\n" );
}

/********************************/
void TestFruitBowlFile ( void ) {

    printf ( "TESTING FRUIT BOWL.\n" );

    FreezeMgr* freezeMgr = FreezeMgr::Get ();

    FruitBowl* file1 = freezeMgr->ThawTyped < FruitBowl >( "fruitBowl.ice" );
    file1->Print ();
    
    freezeMgr->Free ( file1 );
}

/********************************/
// Perform some fruity tests of the FreezeMgr...
void main ( void ) {
    
    RegisterTypes ();

    // Make and a simple linked list of Freezables.
    // Each includes some serial data.
    MakeFruitListFile ();

    // Test the file to make sure the list structure is in tact
    // and the virtual functions all work.
    TestFruitListFile ();

    // Now append another Freezable to the head of the list
    // by opening the file in 'edit' mode.
    AppendTripleMango ();

    // Test the file again.
    TestFruitListFile ();
    
    // Test the case where Freezables are nested inside a non-Freezable.
    MakeFruitBowlFile ();
    TestFruitBowlFile ();
}
