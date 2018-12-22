#include "SCC.H"
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
CodeGen::StartGen( PTNodePtr root )
{
  // Before we can start generating the code in the script, we have to
  // allocate space for any variables that are used in the script.  Since the
  // interpreter does not support scopes, all the variables exist at the base
  // of the stack.  Iterate through the entire symbol table and allocate space
  // for them on the stack.
  //
  // While we are doing this, we need to also need to set the variables
  // address.  This will be important later when the variable is actually
  // used.
  SymbolTable::iterator i = symtbl.begin();
  SymbolTable::iterator end = symtbl.end();

  unsigned int offset = 0;

  for ( ; i != end; ++i ) {
    IdentifierNodePtr var = (*i).second;

    // The interpreter expects the variable offsets to be in word entries;
    // therefore, we only need to increment the offset by one for each
    // variable.
    var->SetOffset( offset );
    ++offset;

    // Emit a dummy push instruction.  This allocates space for the variable
    // on the stack.
    WriteOpcode( Push_Opcode );
    WriteArg( 0 );
  }

  // Now that space for all the variables have been allocated and their
  // offsets are set, generate the script's code.
  Gen( root );

  // To be clean, emit pop instructions to remove the variables that we
  // previously added to the stack.  This will ensure that the stack starts
  // and stops empty.
  for ( i = symtbl.begin(); i != end; ++i )
    WriteOpcode( Pop_Opcode );
}


size_t
CodeGen::Gen( PTNodePtr node )
{
  // This function switches on the node's type and passes flow of control off
  // to the proper internal code generator function.  This helper function
  // will recursively call this function for any children that it may have.

  // It is useful for some of the opcode generators to know how much code has
  // been generated.  Store a marker here when before we generate anything so
  // we can figure out how many bytes was generated for this node.
  Marker start = GetCurrentMarker();

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

    case Identifier_PTNodeType:
      GenIdentifier( IdentifierNodePtr( node ) );
      break;

    case Assignment_PTNodeType:
      GenAssignment( AssignmentNodePtr( node ) );
      break;

    case If_PTNodeType:
      GenIf( IfNodePtr( node ) );
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

  return GetCurrentMarker() - start;
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
CodeGen::GenIdentifier( IdentifierNodePtr identifier )
{
  // An identifier node simply refers that the variable is being "used".  This
  // means its current value should be pushed onto the stack.
  WriteOpcode( Load_Opcode );
  WriteArg( identifier->GetOffset() );
}


void
CodeGen::GenAssignment( AssignmentNodePtr assignment )
{
  Gen( assignment->GetRHS() );

  // An assigment is an expression not a statement.  It needs to leave the
  // result of the assignment on the stack.  This allows the script writer to
  // chain assignments such as:  a = b = 5;
  WriteOpcode( Dupe_Opcode );

  // The left hand side of the assignment node will always be the variable to
  // assign to.  This node's code isn't emited directly.  It is only used to
  // fetch the proper variable offset for the store opcode.
  IdentifierNodePtr var = IdentifierNodePtr( assignment->GetLHS() );

  WriteOpcode( Store_Opcode );
  WriteArg( var->GetOffset() );
}


void
CodeGen::GenIf( IfNodePtr ifNode )
{
  // The assembly for an if-statement with an else block will look like:
  //
  //     Perform the conditional instructions.
  //     If top-stack element is zero, Jump to label A.
  //     Perform true statements.
  //     Jump to label B.
  // A:  Preform false statements.
  // B:
  //
  // The last label, B, will actually point to the next instruction after
  // the if.  An if-statement with out an else block is slightly simplier, it
  // looks like:
  //
  //     Perform the conditional instructions.
  //     If top-stack element is zero, Jump to label A.
  //     Perform true statements.
  // A:
  //
  // Note that the first assembly block will work for both forms of an if
  // statement; however, the second form is slightly more efficient for
  // if-statements with out a false block (it doesn't need the "Jump to B"
  // instruction).


  Marker ifArg;
  size_t trueSize;

  // Generate the if-statement's conditional.  This will leave an argument on
  // the stack that can be used by the IfZero opcode.
  Gen( ifNode->GetConditional() );

  // Emit the IfZero opcode.  If the value on the stack is false, then this
  // opcode will jump to the false statements of the if.  Since the size of
  // the true block isn't known, save a marker so the value can be filled in
  // later.
  WriteOpcode( IfZero_Opcode );
  ifArg = WriteArg( 0 );

  // Generate all of the true statements, and save their size.
  trueSize = Gen( ifNode->GetTrueStatements() );

  // If this if-statement contains any false statements, then emit their code.
  if ( ifNode->GetFalseStatements() != NULL ) {
    Marker jumpArg;
    size_t falseSize;

    // Before the false statements, a relative jump is required to skip these
    // false statements.  This jump is considered part of the true block, and
    // the true statement's size needs to be updated accordingly.
    WriteOpcode( Jump_Opcode );
    jumpArg = WriteArg( 0 );

    trueSize += sizeof( Opcode );
    trueSize += sizeof( int );

    // Generate the if-statements, and update the Jump opcode's argument since
    // we now know how many bytes it needs to skip.
    falseSize = Gen( ifNode->GetFalseStatements() );
    UpdateArg( jumpArg, falseSize );
  }

  // Now that the finial size of the true statement block is known, update
  // IfZero's argument.
  UpdateArg( ifArg, trueSize );
}


void
CodeGen::GenFor( ForNodePtr forStmt )
{
  // The assembly for a for-statement looks like:
  //
  //     Perform pre-loop expression.
  // A:  Perform loop conditional expression.
  //     Jump to label B if top-stack element is zero.
  //     Perform loop body statements.
  //     Perform loop incremental expression.
  //     Jump to label A.
  // B:
  //
  // The last label, B, will actually point to the next isntruction after the
  // for.

  Marker loopTop;
  Marker loopSize;
  Marker ifArg;
  Marker jumpArg;

  // Generate the code for the preloop expression.  Nothing special needs to
  // happen here.
  Gen( forStmt->GetPreLoop() );

  // Save a marker to the top of the loop.  This will be needed to calculate
  // the size of the loop body.
  loopTop = GetCurrentMarker();

  // Generate the conditional part of the for-loop.  Note that we have to
  // generate the IfZero opcodes to actually test the conditional and jump out
  // of the loop, if needed.
  Gen( forStmt->GetConditional() );
  WriteOpcode( IfZero_Opcode );
  ifArg = WriteArg( 0 );

  // Generate the code for the inner part of the function loop (this includes
  // the incremental expression).
  Gen( forStmt->GetLoopBody() );
  Gen( forStmt->GetPostLoop() );

  // Generate the code to jump back up to the conditional.
  WriteOpcode( Jump_Opcode );
  jumpArg = WriteArg( 0 );

  // Now that the entire body of the loop has been generated, calculate the
  // size of the loop body so we can update the jump values.  Note that the
  // jump at the end of the loop needs to go backwards (hence the negative).
  loopSize = GetCurrentMarker() - loopTop;

  UpdateArg( ifArg, loopSize );
  UpdateArg( jumpArg, -loopSize );
}


void
CodeGen::GenStatement( StatementNodePtr stmt )
{
  // A statement yields its entire work to its expression; however, this
  // expression will leave a value on the stack.  For example, the expression
  // "4+5" will leave the answer "9" as the top-most stack element.  Since
  // this statemnt has been completed, it is save to pop this element off the
  // stack.  It is no longer needed.
  //
  // HOWEVER, a statement with no expression does not generate any code (since
  // there is nothing to pop off).  This makes it easy to support empty
  // statements (most notiably for a for-loop).
  if ( stmt == NULL )
    return;

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


CodeGen::Marker
CodeGen::WriteArg( int arg )
{
  // In case the code generator needs to update this opcode argument, return
  // this argument's marker.  We have to fetch the marker now before we update
  // the output file since the marker is supposed to point to the beginning of
  // the argument.
  Marker pos = GetCurrentMarker();

  // Some opcodes (such as Push) expect an argument from the bytecode stream.
  // This function writes out such and argument to the output file.
  fwrite( &arg, sizeof( arg ), 1, out );

  return pos;
}


void
CodeGen::UpdateArg( Marker pos, int arg )
{
  // The marker is really just a file position, so rewind back to the file
  // position, write the new argument, and zip back to the end of the file.
  fseek( out, pos, SEEK_SET );
  WriteArg( arg );
  fseek( out, 0, SEEK_END );
}


CodeGen::Marker
CodeGen::GetCurrentMarker()
{
  // Just use the current offset in the output file as the marker.
  return ftell( out );
}
