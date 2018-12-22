#include "sgx/core/core.hpp"
#include "sgx/filesystem/filesystem.hpp"
#include "sgx/filesystem/io/xml/xml.hpp"

CSGXFile& 
operator<<(CSGXFile&File, const sgxXMLNode *pNode)
{
	int tab = 0;
	sgxString open_tag(sgxXMLBase::GetTab(tab));

	// The <main> tag
	open_tag += "<";
	open_tag += pNode->m_Element.m_Name;

	sgxVector<sgxXMLAttribute>::const_iterator it = pNode->m_Attribute.begin();
	for(;it!=pNode->m_Attribute.end();++it) {
		open_tag += " ";
		open_tag += (*it).m_Name + " = \"" + (*it).m_Value + "\"";
	}
	open_tag += ">";
	File.WriteAsciiStringCR(open_tag);
	
	// The data inside it: generally null, if there are children
	// although some formats do not conform to this part of XML.
	File.WriteAsciiStringCR(sgxXMLBase::GetTab(tab) + pNode->m_Data);

	// The children tags
	sgxVector<sgxXMLNode *>::const_iterator it_node = pNode->m_ChildNodes.begin();

	for(;it_node!=pNode->m_ChildNodes.end();++it_node) {
		File << (*it_node);
	}

	// The </close> tag
	sgxString close_tag(sgxXMLBase::GetTab(tab));
	close_tag += "</";
	close_tag += pNode->m_Element.m_Name;
	close_tag += ">";
	File.WriteAsciiStringCR(close_tag);

	return File;
}