#include <stdio.h>

#include "PTNode.H"


void
PTNode::Dump() const
{
  Dump_Internal( 0 );
  printf( "\n" );

}


void
PTNode::Dump_Internal( int indent ) const
{
  int c = 0;
  for ( ; c < indent; c += 2 )
    printf( " |" );

  printf( " +-- %s\n", GetName().c_str() );

  NodeList::const_iterator i = GetChildrenBegin();
  NodeList::const_iterator end = GetChildrenEnd();

  indent += 2;

  for ( ; i != end; ++i ) {
    PTNodePtr node = *i;

    node->Dump_Internal( indent );
  }

  indent -= 2;
}



// When dumping the parse tree, it is useful to see what the actual constant
// number.  Make sure to add its value to the name string.
string
ConstantNode::GetName() const
{
  char name[64];
  sprintf( name, "Constant %d", m_value );

  return name;
}
