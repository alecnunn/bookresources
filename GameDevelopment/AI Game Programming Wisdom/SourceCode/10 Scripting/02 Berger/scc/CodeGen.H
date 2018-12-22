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
  void StartGen( PTNodePtr root );


protected:
  // This function is called recursively and dispatches the code generation to
  // the proper handler.  This function returns the number of bytes that was
  // generated for this node.
  size_t Gen( PTNodePtr node );

  // These series of functions are used internally to generate the proper
  // instructions for the various types of parse tree nodes.  They help break
  // up the code generation logic.
  void GenAdd( AddNodePtr add );
  void GenSubtract( SubtractNodePtr subtract );
  void GenMultiply( MultiplyNodePtr multiply );
  void GenDivide( DivideNodePtr divide );
  void GenConstant( ConstantNodePtr constant );
  void GenIdentifier( IdentifierNodePtr identifier );
  void GenAssignment( AssignmentNodePtr assignment );
  void GenIf( IfNodePtr ifNode );
  void GenFor( ForNodePtr forNode );
  void GenStatement( StatementNodePtr stmt );
  void GenBlock( BlockNodePtr block );


protected:
  // A marker is simply an abstraction away of the underlying file buffer.
  // This simply notes a spot in the bytecode stream that can be later used to
  // back-patch a value.  This is useful mostly for the jump instructions
  // since they often do not know how far they are going to jump until later
  // in the code generation.
  typedef unsigned int Marker;

  // These two functions handle writing opcodes and an opcodes argument to the
  // bytecode stream.
  void WriteOpcode( Opcode op );
  Marker WriteArg( int arg );

  // This function will update an opcode's argument at the specified marker.
  // While technically, a marker can point to anywhere in the bytecode stream,
  // it only logically makes sense to update opcode arguments.
  void UpdateArg( Marker pos, int arg );

  // This function returns the current marker.
  Marker GetCurrentMarker();


private:
  // This file handle points to the bytecode stream that the code generator is
  // building.
  FILE *out;
};



#endif // __CodeGen_H__
