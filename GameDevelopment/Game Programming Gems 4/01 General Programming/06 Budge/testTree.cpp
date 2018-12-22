/*-----------------------------------------------------------------------------

	testTree.cpp

	Test tree

-----------------------------------------------------------------------------*/


#pragma warning(disable:4786) // long identifier names warning, STL

#include "tree.h"

using namespace std;
using namespace gems;

#include <string>

//-----------------------------------------------------------------------------

static const char* StringToTree(const char* pStr, tree<char>&t)
{
	pStr++;	// skip opening '('
	t.value = *pStr++;
	while (*pStr++ == '(')	// breaks on ')'
	{
		t.push_back(tree<char>());
		pStr = StringToTree(pStr - 1, t.back());
	}
	return pStr;
}

static void TreeToString(const tree<char>&t, string& str)
{
	str += '(';
	str += t.value;
	if (!t.is_leaf())
	{
		tree<char>::const_child_iterator end = t.end_child();
		for (tree<char>::const_child_iterator it = t.begin_child(); it != end; ++it)
			TreeToString(*it, str);
	}
	str += ')';
}

static void CheckTree(const tree<char>& t, char* signature)
{
	string str;
	TreeToString(t, str);
	assert(str == signature);
}

//-----------------------------------------------------------------------------

void TestTreeMutating()
{
	tree<char> test1('A');
	test1.push_back('B');
	tree<char>& bSubtree = test1.back();
	test1.push_back('C');
	tree<char>& cSubtree = test1.back();

	CheckTree(test1, "(A(B)(C))");

	cSubtree.push_back(test1);
	tree<char>& abcSubtree = cSubtree.back();
	CheckTree(test1, "(A(B)(C(A(B)(C))))");

	test1.push_back('D');
	tree<char>& dSubtree = test1.back();
	CheckTree(test1, "(A(B)(C(A(B)(C)))(D))");

	tree<char>::iterator it = test1.insert('D', abcSubtree.begin());
	CheckTree(test1, "(A(B)(C(D)(A(B)(C)))(D))");

	tree<char>::child_iterator childIt(it);
	tree<char>::const_child_iterator constChildIt(it);

	it = abcSubtree.begin();

	(it++)->value = 'E';
	(it++)->value = 'F';
	(it++)->value = 'G';
	CheckTree(test1, "(A(B)(C(D)(E(F)(G)))(D))");

	tree<char> test2(test1);
	assert(test1 == test2);

	tree<char> test3;
	test3 = test2;	// test assignment operator
	assert(test2 == test3);

	bSubtree.splice(bSubtree.begin_child(), cSubtree.begin_child(), cSubtree.end_child());
	CheckTree(test1, "(A(B(D)(E(F)(G)))(C)(D))");
	assert(test1 != test2);

	test1.erase(bSubtree.begin());
	CheckTree(test1, "(A(C)(D))");

	test2.swap(test1);
	CheckTree(test1, "(A(B)(C(D)(E(F)(G)))(D))");
	CheckTree(test2, "(A(C)(D))");

	test1.pop_back();
	CheckTree(test1, "(A(B)(C(D)(E(F)(G))))");
	test1.pop_front();
	CheckTree(test1, "(A(C(D)(E(F)(G))))");

	test2.clear();
	CheckTree(test2, "(A)");
}


void TestTreeNonMutating()
{
	tree<char> test;
	StringToTree("(A(B)(C(D)(E(F)(G))))", test);

	assert(test == test);

	tree<char>& bSubtree = test.front();
	tree<char>& cSubtree = test.back();

	assert(test != bSubtree);

	bool BFromA = bSubtree.is_descendant_of(test);
	assert(BFromA);

	bool CFromB = cSubtree.is_descendant_of(bSubtree);
	assert(!CFromB);

	tree<char>& dSubtree = cSubtree.front();

	unsigned int size = test.size();
	unsigned int degree = test.begin()->degree();
	unsigned int level = dSubtree.level();
	assert((size == 7) && (degree == 2) && (level == 2));
}


void TestTreeIterating()
{
	tree<char> test;
	StringToTree("(A(B)(C(D)(E(F)(G))))", test);

	string forward;
	for (tree<char>::const_iterator f = test.begin(); f != test.end(); ++f)
		forward += f->value;
	assert(forward == "ABCDEFG");

	string reverse;
	for (tree<char>::reverse_iterator r = test.rbegin(); r != test.rend(); ++r)
		reverse += r->value;
	assert(reverse == "GFEDCBA");

	string forward_child;
	for (tree<char>::const_child_iterator fc = test.begin_child(); fc != test.end_child(); ++fc)
		forward_child += fc->value;
	assert(forward_child == "BC");

	string reverse_child;
	for (tree<char>::reverse_child_iterator rc = test.rbegin_child(); rc != test.rend_child(); ++rc)
		reverse_child += rc->value;
	assert(reverse_child == "CB");
}

//-----------------------------------------------------------------------------

#include "StaticAlloc.h"

typedef tree<char, CustomAlloc::StaticAlloc<char, 1024> > CustomAllocTree;

static void TreeToString(const CustomAllocTree& t, string& str)
{
	str += '(';
	str += t.value;
	if (!t.is_leaf())
	{
		CustomAllocTree::const_child_iterator end = t.end_child();
		for (CustomAllocTree::const_child_iterator it = t.begin_child(); it != end; ++it)
			TreeToString(*it, str);
	}
	str += ')';
}

static void CheckTree(const CustomAllocTree& t, char* signature)
{
	string str;
	TreeToString(t, str);
	assert(str == signature);
}

void TestWithNonStandardAllocator()
{
	CustomAllocTree test('A');
	CustomAllocTree::iterator it;
	it = test.begin();
	it->push_back('B');
	it->push_back('C');

	CheckTree(test, "(A(B)(C))");
}

//-----------------------------------------------------------------------------

#include <algorithm>

static string s_string;
static void Append(tree<char>& t)
{
	s_string += t.value;
}

void TestWithSTL()
{
	tree<char> test;
	StringToTree("(A(B)(B)(D)(E))", test);

	for_each(test.begin_child(), test.end_child(), Append);
	assert(s_string == "BBDE");


	unique(test.begin_child(), test.end_child());
	CheckTree(test, "(A(B)(D)(E)(E))");

	reverse(test.begin_child(), test.end_child());
	CheckTree(test, "(A(E)(E)(D)(B))");
}

//-----------------------------------------------------------------------------

void TestTree()
{
	TestTreeMutating();
	TestTreeNonMutating();

	TestTreeIterating();

	TestWithNonStandardAllocator();

	TestWithSTL();
}

