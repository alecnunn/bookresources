#include <gpl/gpl.h>

int main(int argc, char *argv[])
{
	GPL_Tree tree;
	for(int i=0;i<100;i++)
	{
		GPL_TreeNode *n = new GPL_TreeNode(rand());
		tree.Insert(n);
	}
	tree.InOrder(tree.Root());
	return 0;
}

