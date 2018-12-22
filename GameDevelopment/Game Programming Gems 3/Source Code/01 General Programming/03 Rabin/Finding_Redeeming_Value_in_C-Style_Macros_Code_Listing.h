
//////////////////////////////////////////////////////////////////////////////////////
// Macros as described in the article "Finding Redeeming Value in C-Style Macros" by
// Steve Rabin within the book Game Programming Gems 3.
//
// Macros that do not compile "as is" have been commented out.
//

#ifndef __MACROS__
#define __MACROS__

#include <assert.h>

////////////////////////////////////////////////////////////////////////
//Macro Trick #1: Turning Enums into Strings

/*
#define CaseEnum(a)   case(a): LogMsgToFile(#a, id, time)

switch( msg_passed_in )
{
  CaseEnum( MSG_YouWereHit );
    ReactToHit();
    break;

  CaseEnum( MSG_GameReset );
    ResetLogic();
    break;
}
*/

/*
// data.h

DATA(MSG_YouWereHit)
DATA(MSG_GameReset)
DATA(MSG_HealthRestored)

// data.cpp

#define DATA(x) x,

enum GameMessages
{
  #include "data.h"
};

#undef DATA
#define DATA(x) #x, // make enums into strings

static const char* GameMessageNames[] =
{
  #include "data.h"
};

#undef DATA
*/



////////////////////////////////////////////////////////////////////////
//Macro Trick #2: Compile-Time Constants from Binary Representations
// Thanks to Jeff Grills for providing this implementation

/*
//Usage
const int nibble = BINARY1(0101);       // 0x5
const int byte   = BINARY2(1010,0101);  // 0xa5

// 0xa5a5a5a5
const int dword = BINARY8(1010,0101,1010,0101,1010,0101,1010,0101);
*/

//Macro source:

#define HEX_DIGIT_0000 0
#define HEX_DIGIT_0001 1
#define HEX_DIGIT_0010 2
#define HEX_DIGIT_0011 3
#define HEX_DIGIT_0100 4
#define HEX_DIGIT_0101 5
#define HEX_DIGIT_0110 6
#define HEX_DIGIT_0111 7
#define HEX_DIGIT_1000 8
#define HEX_DIGIT_1001 9
#define HEX_DIGIT_1010 a
#define HEX_DIGIT_1011 b
#define HEX_DIGIT_1100 c
#define HEX_DIGIT_1101 d
#define HEX_DIGIT_1110 e
#define HEX_DIGIT_1111 f

#define HEX_DIGIT(a)        HEX_DIGIT_ ## a

#define BINARY1H(a)         (0x ## a)
#define BINARY1I(a)         BINARY1H(a)
#define BINARY1(a)          BINARY1I(HEX_DIGIT(a))

#define BINARY2H(a,b)       (0x ## a ## b)
#define BINARY2I(a,b)       BINARY2H(a,b)
#define BINARY2(a,b)        BINARY2I(HEX_DIGIT(a), HEX_DIGIT(b))

#define BINARY8H(a,b,c,d,e,f,g,h) (0x##a##b##c##d##e##f##g##h)
#define BINARY8I(a,b,c,d,e,f,g,h) BINARY8H(a,b,c,d,e,f,g,h)
#define BINARY8(a,b,c,d,e,f,g,h)  BINARY8I(HEX_DIGIT(a),    \
 HEX_DIGIT(b), HEX_DIGIT(c), HEX_DIGIT(d), HEX_DIGIT(e),    \
 HEX_DIGIT(f), HEX_DIGIT(g), HEX_DIGIT(h))



////////////////////////////////////////////////////////////////////////
//Macro Trick #3: Adding a Descriptive Comment to Standard Assert

#define assertmsg(a,b) assert( a && b )



////////////////////////////////////////////////////////////////////////
//Macro Trick #4: Compile-Time Assert

#define cassert(expn) typedef char __C_ASSERT__[(expn)?1:-1]



////////////////////////////////////////////////////////////////////////
//Macro Trick #5: Determining the Number of Elements in an Array

#define NumElm(array) (sizeof(array) / sizeof((array)[0]))



////////////////////////////////////////////////////////////////////////
//Macro Trick #6: Making __LINE__ into a String

//Does not work correctly with Microsoft Visual C++
//Works with Metrowerks CodeWarrior and SN Systems ProDG (gcc compiler)
#define _QUOTE(x) # x
#define QUOTE(x) _QUOTE(x)
#define __FILELINE__ __FILE__ "(" QUOTE(__LINE__) ")"



////////////////////////////////////////////////////////////////////////
//Macro Trick #7: Protecting against Infinite Loops

//Usage: while_limit( expression, limit ) { }

//Works on Microsoft Visual C++ and Metrowerks CodeWarrior (not gcc compilers)
static bool while_assert( bool a )
{
  assert( a && "while_limit: exceeded iteration limit" );
  return( a );
}

#define UNIQUE_VAR(x) safety_limit ## x
#define _while_limit(a,b,c) \
  assert(b>0 && "while_limit: limit is zero or negative");\
  int UNIQUE_VAR(c) = b; \
  while(a && while_assert(--UNIQUE_VAR(c)>=0))
#define while_limit(a,b) _while_limit(a,b,__COUNTER__)

/*
//Alternative while_limit macro that works on SN Systems ProDG and Metrowerks CodeWarrior
static bool while_assert( bool a )
{
  assert( a && "while_limit: exceeded iteration limit" );
  return( a );
}

#define _UNIQUE_VAR(x) safety_limit ## x
#define UNIQUE_VAR(x) _UNIQUE_VAR(x)
#define while_limit(a,b) \
  assert(b>0 && "while_limit: limit is zero or negative"); \
  int UNIQUE_VAR(__LINE__) = b; \
  while(a && while_assert(--UNIQUE_VAR(__LINE__)>=0))
*/


////////////////////////////////////////////////////////////////////////
//Macro Trick #8: Small Specialized Languages
// Refer to "Implementing a State Machine Language" in
// the book AI Game Programming Wisdom

#define BeginStateMachine  if(state < 0){if(0){
#define EndStateMachine    return(true);}}else{assert(0); \
                           return(false);}return(false);
#define State(a)           return(true);}} \
                           else if(a == state){if(0){
#define OnEvent(a)         return(true);}else if(a == event){
#define OnEnter            OnEvent(EVENT_Enter)
#define OnUpdate           OnEvent(EVENT_Update)
#define OnExit             OnEvent(EVENT_Exit)



////////////////////////////////////////////////////////////////////////
//Macro Trick #9: Simplifying Class Interfaces
// Thanks to Paul Tozour for providing this macro trick

/*
#define INTERFACE_Creature(terminal)               \
  public:                                          \
	virtual std::string GetName() const ##terminal \
	virtual int GetHitPoints() const ##terminal    \
	virtual float GetMaxVelocity() const ##terminal

#define BASE_Creature		INTERFACE_Creature(=0;)
#define DERIVED_Creature	INTERFACE_Creature(;)
*/


#endif //__MACROS__
