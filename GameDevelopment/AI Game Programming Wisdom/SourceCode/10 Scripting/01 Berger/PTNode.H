#ifndef __PTNode_H__
#define __PTNode_H__


#include <string>
#include <vector>

#include "SmartPtr.H"

// This file defines all the various parse tree (or PT) nodes that are used by
// the compiler.  Every parse tree contains the same basic structure:  a type
// and a set of children.  Because of this, the base parse tree class, PTNode,
// has an enumeration which describes the type of parse tree node, and an
// array of children.
//
// Since most parse tree nodes will have a very fixed number of children
// (e.g., binary operators will always have two children), it is convient to
// derive utility classes from this base parse tree node.  This also has the
// advantage of hiding the parse tree enumeration from the parser, and
// constructors can be used to full effect.


// The parser tends to through around pointers a lot.  Instead of having to
// keep track of when to delete memory, define a bunch of smart pointers.
// Since these smart pointers are reference counted, they will delete the
// memory automatically.
typedef SmartPtr<class PTNode> PTNodePtr;
typedef SmartPtr<class AddNode> AddNodePtr;
typedef SmartPtr<class SubtractNode> SubtractNodePtr;
typedef SmartPtr<class MultiplyNode> MultiplyNodePtr;
typedef SmartPtr<class DivideNode> DivideNodePtr;
typedef SmartPtr<class ConstantNode> ConstantNodePtr;
typedef SmartPtr<class StatementNode> StatementNodePtr;
typedef SmartPtr<class BlockNode> BlockNodePtr;


// Define a typedef that will keep track of the set of children nodes that a
// parse tree node can contain.
using namespace std;
typedef vector<PTNodePtr> NodeList;


// Define the enumeration for the parse tree node types.  Every type of parse
// tree requires a unique type in here.
enum PTNodeType {
  Undef_PTNodeType,

  Add_PTNodeType,
  Subtract_PTNodeType,
  Multiply_PTNodeType,
  Divide_PTNodeType,

  Constant_PTNodeType,

  Statement_PTNodeType,
  Block_PTNodeType
};



// Below are the various parse tree nodes that are used by the compiler.  Only
// the root node contains the type enumeration and the set of children.
// Derived classes simply override the constructor to allow easy construction
// of the specific nodes.  They also add any helpful utiltiy functions for the
// code generator to use.

class PTNode : public RefCount {
public:
  PTNode( PTNodeType type )
  {
    m_type = type;
  }

  virtual string GetName() const = 0;

  void Add( PTNodePtr node )
  {
    m_children.push_back( node );
  }

  PTNodeType GetType() const
  {
    return m_type;
  }

  // This dump function writes to stdout the enitre contents of this parse
  // tree.
  void Dump() const;


protected:
  // The code generator needs to be able to iterate over the children that are
  // contains in this parse tree node.  Define a few accessors so it can get
  // to the iterators.
  friend class CodeGen;

  NodeList::const_iterator GetChildrenBegin() const
  {
    return m_children.begin();
  }

  NodeList::const_iterator GetChildrenEnd() const
  {
    return m_children.end();
  }


protected:
  // This function is called recursively by Dump() to dump the entire contents
  // of the parse tree.  'indent' is the current indentation level.
  void Dump_Internal( int indent ) const;

  // Define the two data members for the parse tree:  The type and the set of
  // children for this node.
  PTNodeType m_type;
  NodeList m_children;
};



// The binary operator node is not used directly by the parser.  This class
// just defines some functions that are in common with all binary operators.
class BinOpNode : public PTNode {
public:
  BinOpNode( PTNodeType type, PTNodePtr lhs, PTNodePtr rhs )
    : PTNode( type )
  {
    Add( lhs );
    Add( rhs );
  }

  const PTNodePtr GetLHS() const                  { return m_children[0]; }
  const PTNodePtr GetRHS() const                  { return m_children[1]; }
};


class AddNode : public BinOpNode {
public:
  AddNode( PTNodePtr lhs, PTNodePtr rhs )
    : BinOpNode( Add_PTNodeType, lhs, rhs )
  {
  }

  virtual string GetName() const                  { return "Add"; }
};


class SubtractNode : public BinOpNode {
public:
  SubtractNode( PTNodePtr lhs, PTNodePtr rhs )
    : BinOpNode( Subtract_PTNodeType, lhs, rhs )
  {
  }

  virtual string GetName() const                  { return "Subtract"; }
};


class MultiplyNode : public BinOpNode {
public:
  MultiplyNode( PTNodePtr lhs, PTNodePtr rhs )
    : BinOpNode( Multiply_PTNodeType, lhs, rhs )
  {
  }

  virtual string GetName() const                  { return "Multiply"; }
};


class DivideNode : public BinOpNode {
public:
  DivideNode( PTNodePtr lhs, PTNodePtr rhs )
    : BinOpNode( Divide_PTNodeType, lhs, rhs )
  {
  }

  virtual string GetName() const                  { return "Divide"; }
};


// The constant node needs to also keep track of the constant that was
// specified in script.  This is done with the data member 'm_value'.
class ConstantNode : public PTNode {
public:
  ConstantNode( int value )
    : PTNode( Constant_PTNodeType ),
      m_value( value )
  {
  }

  int GetValue() const                            { return m_value; }

  virtual string GetName() const;


private:
  int m_value;
};


class StatementNode : public PTNode {
public:
  StatementNode( PTNodePtr expr )
    : PTNode( Statement_PTNodeType )
  {
    Add( expr );
  }

  const PTNodePtr GetExpression() const           { return m_children[0]; }

  virtual string GetName() const                  { return "Statement"; }
};


class BlockNode : public PTNode {
public:
  BlockNode( PTNodePtr stmt )
    : PTNode( Block_PTNodeType )
  {
    Add( stmt );
  }

  virtual string GetName() const                  { return "Block"; }
};


#endif // __PTNode_H__
