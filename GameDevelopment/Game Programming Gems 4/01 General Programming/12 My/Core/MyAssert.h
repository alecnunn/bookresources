//--------------------------------------------------------------------------------------------------------------------//
//                                                ASSERT CUSTOM MACRO                                                 //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//                                                                                                                    //
//           based on item 1.12 of the Game Programming Gems book                                                     //
// features: understandable error message (YOU should provide it)                                                     //
//           breaks on the line where MY_ASSERT sits, not in assert.c                                                 //
//           "ignore always" option                                                                                   //
//           the whole dialog text is copied to the clipboard for easier bug reporting                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_MYASSERT_H_
#define   _GPG4_MYASSERT_H_

//----------------------------------------------- MACROS -------------------------------------------------------------//

#define   _ASSERT_ACTIVE_
//#undef    _ASSERT_ACTIVE_

#ifdef    _ASSERT_ACTIVE_
namespace  GPG4
  {
  extern   bool DisplayAssert(char*,char*,int,bool*);       // display message & choose to continue/abort (false) or debug (true)
  }

#define   MY_ASSERT(cond,text)                                         \
  {                                                                    \
  static bool boIgnoreAlways = false;                                  \
  if(!boIgnoreAlways && !(cond))                                       \
    {                                                                  \
    if(GPG4::DisplayAssert(text,__FILE__,__LINE__,&boIgnoreAlways))    \
      {                                                                \
      _asm { int 3 }                                                   \
      }                                                                \
    }                                                                  \
  }

#else  // _ASSERT_ACTIVE_
#define   MY_ASSERT(cond,text)  \
  {                             \
  if(cond) {}                   \
  }
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_MYASSERT_H_
