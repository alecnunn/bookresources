#ifndef __VM_H__
#define __VM_H__

#include <vector>

#include "Opcode.H"


// The VM class defines a stack-based implementation of an interpreter.  The
// bytecode stream to execute is passed in to this class's constructor.  Once
// the Exec() member function is called, the class will handle executing all
// the specified bytecode stream.  Execution will continue until the entire
// bytecode stream has been process.
class VM {
public:
  VM( const char *stream, size_t size );
  ~VM();

  void Exec();


protected:
  // Below are all of the various opcode handlers.  They are responsible for
  // actually evaluating the bytecode stream.  They will modify the contents
  // of the stack as needed.

  bool HandleBinOp( Opcode op );

  bool HandlePush( Opcode op );
  bool HandlePop( Opcode op );
  bool HandleDupe( Opcode op );

  bool HandleLoad( Opcode op );
  bool HandleStore( Opcode op );

  bool HandleJump( Opcode op );
  bool HandleIfZero( Opcode op );


protected:
  // This function returns the new opcode in the bytecode stream.  This
  // implicitly increments the instruction pointer to point to next
  // opcode.
  Opcode GetNextOpcode();

  // Some opcodes contain arguments in the bytecode stream.  This function
  // fetches the opcode's argument and increments the instruction pointer to
  // skip the argument.
  int GetOpcodeArg();

  // The opcode handlers call this function to spew some text to stdout along
  // with the current stack contents.  By convention, the given string should
  // not contain a newline, and this function should be called once the opcode
  // handler is finished modifing the stack.
  void Trace( char *fmt, ... );


protected:
  // These two functions simply hide the stack pushing/poping.  The main
  // reason they exist is to make the std::vector interface a little easier to
  // deal with.  Mainly because it is convient for Pop() to actually the int
  // that was popped off.
  void Push( int arg );
  int Pop();


private:
  // Define the data member that contains all of the function pointers to the
  // various opcode handlers.  The index into this array is the actual opcode's
  // value.
  typedef bool ( VM::*OpcodeHandler )( Opcode );
  OpcodeHandler m_opHandlers[ Num_Opcode ];

  // Define a VM's actual execution stack.  Note that the load and store
  // instructions need to be able to modify arbitrary locations in the stack;
  // therefore, it is more efficient to use a vector than a stack.
  typedef std::vector<int> RuntimeStack;
  RuntimeStack m_stack;

  // The instruction pointer points to the next instruction to exectue.
  const char *m_ip;

  // These two data members keeps track of the bytecode stream that the VM
  // should execute.
  const char *m_stream;
  size_t m_streamSize;
};


#endif // __VM_H__
