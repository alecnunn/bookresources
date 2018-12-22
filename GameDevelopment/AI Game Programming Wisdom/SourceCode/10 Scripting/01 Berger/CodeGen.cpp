#include <assert.h>

#include "CodeGen.H"
#include "PTNode.H"


CodeGen::CodeGen()
{
  // When this class is instantiated, open the 'out.bin' file.  Obviously, a
  // real compiler would want the user to specify the output file name as a
  // command-line argument.
  out = fopen( "out.bin", "wb" );
  if ( out == NULL ) {
    perror( "out.bin" );
    exit( 1 );
  }
}


CodeGen::~CodeGen()
{
  // Make sure to close the bytecode stream.
  fclose( out );
}


void
CodeGen::Gen( PTNodePtr node )
{
  // This function switches on the node's type and passes flow of control off
  // to the proper internal code generator function.  This helper function
  // will recursively call this function for any children that it may have.


  switch ( node->GetType() ) {
    case Add_PTNodeType:
      GenAdd( AddNodePtr( node ) );
      break;

    case Subtract_PTNodeType:
      GenSubtract( SubtractNodePtr( node ) );
      break;

    case Multiply_PTNodeType:
      GenMultiply( MultiplyNodePtr( node ) );
      break;

    case Divide_PTNodeType:
      GenDivide( DivideNodePtr( node ) );
      break;

    case Constant_PTNodeType:
      GenConstant( ConstantNodePtr( node ) );
      break;

    case Statement_PTNodeType:
      GenStatement( StatementNodePtr( node ) );
      break;

    case Block_PTNodeType:
      GenBlock( BlockNodePtr( node ) );
      break;

    default:
      // Some unknown node type!  This shouldn't happen...
      assert( false );
      break;
  }
}


void
CodeGen::GenAdd( AddNodePtr add )
{
  // All of the basic math nodes have the same format:  Generate the code for
  // the left & right hand expressions, and then emit the proper opcode
  // instruction.  This makes sure that the stack will contain the proper two
  // values to preform the addition, subtraction, etc.
  Gen( add->GetLHS() );
  Gen( add->GetRHS() );

  WriteOpcode( Add_Opcode );
}


void
CodeGen::GenSubtract( SubtractNodePtr subtract )
{
  Gen( subtract->GetLHS() );
  Gen( subtract->GetRHS() );

  WriteOpcode( Subtract_Opcode );
}


void
CodeGen::GenMultiply( MultiplyNodePtr multiply )
{
  Gen( multiply->GetLHS() );
  Gen( multiply->GetRHS() );

  WriteOpcode( Multiply_Opcode );
}


void
CodeGen::GenDivide( DivideNodePtr divide )
{
  Gen( divide->GetLHS() );
  Gen( divide->GetRHS() );

  WriteOpcode( Divide_Opcode );
}


void
CodeGen::GenConstant( ConstantNodePtr constant )
{
  // A constant simply pushes its own value onto the stack.  The Push opcode
  // expects that the next word in the bytecode stream is the value to push
  // onto the stack.
  WriteOpcode( Push_Opcode );
  WriteArg( constant->GetValue() );
}


void
CodeGen::GenStatement( StatementNodePtr stmt )
{
  // A statement yields its entire work to its expression; however, this
  // expression will leave a value on the stack.  For example, the expression
  // "4+5" will leave the answer "9" as the top-most stack element.  Since
  // this statemnt has been completed, it is save to pop this element off the
  // stack.  It is no longer needed.
  Gen( stmt->GetExpression() );

  WriteOpcode( Pop_Opcode );
}


void
CodeGen::GenBlock( BlockNodePtr block )
{
  // Block nodes do not have any code to generate.  They simply iterate over
  // all their children making sure that their values are generated.
  NodeList::const_iterator i = block->GetChildrenBegin();
  NodeList::const_iterator end = block->GetChildrenEnd();

  for ( ; i != end; ++i ) {
    const PTNodePtr node = *i;

    Gen( node );
  }
}


void
CodeGen::WriteOpcode( Opcode op )
{
  // Write out opcodes as if they were an integer.  If the size of the
  // generate bytecode stream is an issue, then you'll want to only
  // write out the smallest amount possible (probably a byte).
  WriteArg( (int)op );
}


void
CodeGen::WriteArg( int arg )
{
  // Some opcodes (such as Push) expect an argument from the bytecode stream.
  // This function writes out such and argument to the output file.
  fwrite( &arg, sizeof( arg ), 1, out );
}
