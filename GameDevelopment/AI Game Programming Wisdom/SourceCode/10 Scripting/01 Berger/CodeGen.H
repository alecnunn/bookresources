#ifndef __CodeGen_H__
#define __CodeGen_H__

#include <stdio.h>

#include "Opcode.H"
#include "PTNode.H"


// This is the code generator class.  Once the parser has completed building
// the parse tree, it passes the tree into this class to generate the bytecode
// stream.  This class simply needs to iterate through the nodes in the parse
// tree and generate all the proper instructions.
class CodeGen {
public:
  CodeGen();
  ~CodeGen();

  // This function is the only public API to this class.  It expects the root
  // of the parse tree that it will generate the bytecode stream from.
  void Gen( PTNodePtr node );


protected:
  // These series of functions are used internally to generate the proper
  // instructions for the various types of parse tree nodes.  These functions
  // help break up the code generation logic.
  void GenAdd( AddNodePtr add );
  void GenSubtract( SubtractNodePtr subtract );
  void GenMultiply( MultiplyNodePtr multiply );
  void GenDivide( DivideNodePtr divide );
  void GenConstant( ConstantNodePtr constant );
  void GenStatement( StatementNodePtr stmt );
  void GenBlock( BlockNodePtr block );

  // These two functions handle writing opcodes and an opcodes argument to the
  // bytecode stream.
  void WriteOpcode( Opcode op );
  void WriteArg( int arg );

private:
  // This file handle points to the bytecode stream that the code generator is
  // building.
  FILE *out;
};



#endif // __CodeGen_H__
