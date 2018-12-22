#ifndef __Opcode_H__
#define __Opcode_H__


// This enumeration defines the set of opcodes that the interpreter
// understands.  The code generator needs to use this enumeration to generate
// the proper bytecode stream.
enum Opcode {
  // Define the four basic math opcodes.
  Add_Opcode,
  Subtract_Opcode,
  Multiply_Opcode,
  Divide_Opcode,

  // Push & pop add and remove a value from the interpreter's stack.
  Push_Opcode,
  Pop_Opcode,

  // This simply defines the maximum number of opcodes used.
  Num_Opcode
};


#endif // __Opcode_H__
