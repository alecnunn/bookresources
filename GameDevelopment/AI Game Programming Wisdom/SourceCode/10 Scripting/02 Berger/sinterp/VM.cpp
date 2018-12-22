#include <stdio.h>
#include <stdarg.h>

#include "VM.H"



VM::VM( const char *stream, size_t size )
{
  // Setup the bytecode stream data members to point to the bytecode stream
  // that the interpreter is supposed to execute.
  m_stream = stream;
  m_streamSize = size;

  // Setup each of the opcode handlers.
  m_opHandlers[Add_Opcode]      = HandleBinOp;
  m_opHandlers[Subtract_Opcode] = HandleBinOp;
  m_opHandlers[Multiply_Opcode] = HandleBinOp;
  m_opHandlers[Divide_Opcode]   = HandleBinOp;

  m_opHandlers[Push_Opcode]     = HandlePush;
  m_opHandlers[Pop_Opcode]      = HandlePop;
  m_opHandlers[Dupe_Opcode]     = HandleDupe;

  m_opHandlers[Load_Opcode]     = HandleLoad;
  m_opHandlers[Store_Opcode]    = HandleStore;

  m_opHandlers[Jump_Opcode]     = HandleJump;
  m_opHandlers[IfZero_Opcode]   = HandleIfZero;
}


VM::~VM()
{
}



void
VM::Exec()
{
  // Make sure that instruction pointer starts at the beginning of the
  // bytecode stream.  This is needed in case the caller constructs a single
  // VM class and wants to execute the same bytecode stream multiple times.
  m_ip = m_stream;

  for ( ;; ) {
    // Get the next opcode in the stream.  If there are no opcodes left, then
    // Num_Opcode will be returned.  Break out of the loop since there is
    // nothing left to do.
    Opcode op = GetNextOpcode();
    if ( op == Num_Opcode )
      break;

    // Fetch the proper opcode handler from our function pointer table and
    // pass the flow of control to the handler.  If this handler returns
    // false, then something bad has happened.  A real VM will raise errors at
    // this point (and maybe even drop a core?).
    OpcodeHandler handler = m_opHandlers[op];

    if ( !(this->*handler)( op ) )
      break;
  }
}


Opcode
VM::GetNextOpcode()
{
  // The instruction pointer always points to the next instruction to
  // execute; therefore, it is possible that it points outside of the bytecode
  // stream.  If this happens, then we've reached the end of the instructions
  // we are supposed to execute.
  const char *end = m_stream + m_streamSize;

  if ( m_ip >= end )
    return Num_Opcode;

  // Fetch the next opcode from the bytecode stream and increment the
  // instruction pointer.
  Opcode op = *(Opcode *)m_ip;
  m_ip += sizeof( Opcode );

  return op;
}


int
VM::GetOpcodeArg()
{
  // Fetch the opcode's argument from the bytecode stream and increment the
  // instruction pointer.
  int arg = *(int *)m_ip;
  m_ip += sizeof( int );

  return arg;
}


bool
VM::HandleBinOp( Opcode op )
{
  char *type;
  int result;

  // All binary operations have two arguments; therefore, pop off the left and
  // right hands sides of the operation.  Note that the order here is
  // important, and this is the same order that the code generator will push
  // the values onto the stack.
  int rhs = Pop();
  int lhs = Pop();


  // Ensure the script doesn't try to divide by zero!
  if ( op == Divide_Opcode && rhs == 0 ) {
    printf( "division by zero.\n" );
    return false;
  }


  switch ( op ) {
    case Add_Opcode:        type = "+";   result = lhs + rhs;   break;
    case Subtract_Opcode:   type = "-";   result = lhs - rhs;   break;
    case Multiply_Opcode:   type = "*";   result = lhs * rhs;   break;
    case Divide_Opcode:     type = "/";   result = lhs / rhs;   break;
  }


  Push( result );

  Trace( "%d %s %d", lhs, type, rhs );

  return true;
}


bool
VM::HandlePush( Opcode op )
{
  int arg = GetOpcodeArg();

  Push( arg );

  Trace( "pushing %d", arg );

  return true;
}


bool
VM::HandlePop( Opcode op )
{
  int val = Pop();

  Trace( "pop %d", val );

  // In this sample compiler, pop will be generated at the end of a statement.
  // This blank line makes it easier to match up statements with a script.
  printf( "\n" );

  return true;
}


bool
VM::HandleDupe( Opcode op )
{
  int val = Pop();

  Push( val );
  Push( val );

  Trace( "duping %d", val );

  return true;
}


bool
VM::HandleLoad( Opcode op )
{
  int arg = GetOpcodeArg();

  Push( m_stack[arg] );

  Trace( "loading %d", arg );

  return true;
}


bool
VM::HandleStore( Opcode op )
{
  int arg = GetOpcodeArg();
  int val = Pop();

  m_stack[arg] = val;

  Trace( "storing %d at %d", val, arg );

  return true;
}


bool
VM::HandleJump( Opcode op )
{
  int arg = GetOpcodeArg();

  // Since this jump opcode is a relative jump, add the opcode's argument to
  // the instruction pointer.
  m_ip += arg;

  Trace( "jumping %d", arg );

  return true;
}


bool
VM::HandleIfZero( Opcode op )
{
  int arg = GetOpcodeArg();
  int val = Pop();

  // If the value on the stack is zero, then increment the instruction pointer
  // by the specified amount.x
  if ( val == 0 )
    m_ip += arg;

  Trace( "if %d == 0 then %d", val, arg );

  return true;
}


void
VM::Trace( char *fmt, ... )
{
  va_list args;

  char buf[128];
  char out[128];
  size_t pos;

  // Build up a buffer with the opcode's spew.
  va_start( args, fmt );
  vsprintf( buf, fmt, args );
  va_end( args );

  // Iterate through the stack and dump its contents.
  pos = sprintf( out, "%-25s -=-  ", buf );

  RuntimeStack::iterator i = m_stack.begin();
  RuntimeStack::iterator end = m_stack.end();

  for ( ; i != end; ++i )
    pos += sprintf( &out[pos], " %2d", *i );

  puts( out );
}


void
VM::Push( int arg )
{
  m_stack.push_back( arg );
}


int
VM::Pop()
{
  int arg = m_stack.back();
  m_stack.pop_back();

  return arg;
}
