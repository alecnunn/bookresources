#ifndef __Opcodes_H__
#define __Opcodes_H__


// This enumeration defines the set of opcodes that the interpreter
// understands.  The code generator needs to use this enumeration to generate
// the proper bytecode stream, and the interpreter uses this enumeration to
// actually evaluate the instructions.
enum Opcode {
  // Define the four basic math opcodes.
  Add_Opcode,
  Subtract_Opcode,
  Multiply_Opcode,
  Divide_Opcode,

  // Push & pop add and remove a value from the interpreter's stack.
  Push_Opcode,
  Pop_Opcode,

  // The dupe opcode takes the stack element and pushes a copy of it onto the
  // stack.
  Dupe_Opcode,

  // These two opcodes loads and stores a variable off of the stack.  The
  // opcode's argument is the offset (in stack words) from the first element
  // in the stack where the variable exists.
  Load_Opcode,
  Store_Opcode,

  // Increments the instruction pointer by the opcode's argument (i.e., a
  // relative jump).
  Jump_Opcode,

  // Pops the stack element off and increments the instruction pointer by
  // the opcode's argument if the stack element is zero (i.e., performs a
  // relative jump on zero).
  IfZero_Opcode,

  // This simply defines the maximum number of opcodes used.
  Num_Opcode
};


#endif // __Opcodes_H__
