#ifndef SGX_CORE_XML_TREE_HPP
#define SGX_CORE_XML_TREE_HPP	1

class sgxXMLTree;
typedef tBOOL (*cbfn_TreeTraverse)(const sgxXMLTree *pTree, const sgxXMLNode *pNode, tMEMSIZE param);

class sgxXMLTreeTraverse {
public:
	sgxXMLTree	*m_pTree;
	sgxXMLNode	*m_pNode;
	tMEMSIZE	m_Param;
};

class sgxXMLTree {
public:
	sgxXMLTree();
	~sgxXMLTree();

	sgxXMLNode			*GetRootNode() { return m_pRoot; }
	const sgxXMLNode	*GetRootNode() const { return m_pRoot; }

	tBOOL	Traverse(cbfn_TreeTraverse cb, tMEMSIZE param) const;

protected:
	sgxXMLNode	*m_pRoot;

private:
	tBOOL	TraverseNode(const sgxXMLNode *pNode, cbfn_TreeTraverse cb, tMEMSIZE param) const;
};

#endif /* SGX_CORE_XML_TREE_HPP */
