#include "sgx/core/core.hpp"
#include "sgx/core/xml/xmltree.hpp"

sgxXMLTree::sgxXMLTree()
{
	m_pRoot = new sgxXMLNode;
	m_pRoot->SetElement("xml");
	m_pRoot->AddAttribute("version", "1.0");
	m_pRoot->AddAttribute("encoding", "UTF-8");
}

sgxXMLTree::~sgxXMLTree()
{
	delete	m_pRoot;
}

tBOOL sgxXMLTree::Traverse(cbfn_TreeTraverse cb, tMEMSIZE param) const
{
	return TraverseNode(GetRootNode(), cb, param);
}

tBOOL sgxXMLTree::TraverseNode(const sgxXMLNode *pNode, cbfn_TreeTraverse cb, tMEMSIZE param) const
{
	sgxVector<sgxXMLNode *>::const_iterator it = pNode->m_ChildNodes.begin();

	for(;it!=pNode->m_ChildNodes.end();++it) {
		// Return FALSE to ignore the children of this node
		// This allows the callback to handle the node in its entirety
		// To terminate the whole tree, the cb should set a flag and return
		// FALSE on every invocation.
		if ((*cb)(this, (*it), param)) {
			TraverseNode((*it), cb, param);
		}
	}

	return TRUE;
}
