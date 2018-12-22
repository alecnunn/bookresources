#include "sgx/core/core.hpp"
#include "sgx/core/xml/xmlbase.hpp"

sgxXMLNode::~sgxXMLNode()
{
	sgxVector<sgxXMLNode *>::iterator it = m_ChildNodes.begin();

	for(;it!=m_ChildNodes.end();++it) {
		delete (*it);
	}
}

const sgxXMLNode *sgxXMLNode::GetChild(const sgxString &name) const
{
	sgxVector<sgxXMLNode *>::const_iterator it = m_ChildNodes.begin();

	for(;it!=m_ChildNodes.end();++it) {
		if ((*it) && (*it)->m_Element.m_Name == name) {
			return (*it);
		}
	}

	return NULL;
}

sgxXMLNode *sgxXMLNode::GetChild(const sgxString &name)
{
	sgxVector<sgxXMLNode *>::iterator it = m_ChildNodes.begin();

	for(;it!=m_ChildNodes.end();++it) {
		if ((*it) && (*it)->m_Element.m_Name == name) {
			return (*it);
		}
	}

	return NULL;
}

tBOOL	sgxXMLNode::DoesAttributeExist(const sgxString &name) const
{
	sgxString v;
	return GetAttributeImpl(name, v);
}

tBOOL	sgxXMLNode::GetAttributeImpl(const sgxString &name, sgxString &value) const
{
	sgxVector<sgxXMLAttribute>::const_iterator it = m_Attribute.begin();

	for(;it!=m_Attribute.end();++it) {
		if ((*it).m_Name == name) {
			value = (*it).m_Value;
			return TRUE;
		}
	}

	return FALSE;
}

//
// GetAttribute: These versions will apply a default value, if no tag is found.
//
tBOOL	sgxXMLNode::GetAttribute(const sgxString &name, sgxString &value, const sgxString &default_value) const
{
	if (!GetAttributeImpl(name, value)) {
		value = default_value;
		return FALSE;
	}

	return TRUE;
}

tBOOL	sgxXMLNode::GetAttribute(const sgxString &name, tUINT32 &value, const tUINT32 default_value) const
{
	sgxString str;

	if (!GetAttributeImpl(name, str)) {
		value = default_value;
		return FALSE;
	}

	value = atoi(str.c_str());
	return TRUE;
}

tBOOL	sgxXMLNode::GetAttribute(const sgxString &name, tINT32 &value, const tINT32 default_value) const
{
	sgxString str;

	if (!GetAttributeImpl(name, str)) {
		value = default_value;
		return FALSE;
	}

	value = atoi(str.c_str());
	return TRUE;
}

tBOOL	sgxXMLNode::GetAttribute(const sgxString &name, tREAL32 &value, const tREAL32 default_value) const
{
	sgxString str;

	if (!GetAttributeImpl(name, str)) {
		value = default_value;
		return FALSE;
	}

	value = (tREAL32)atof(str.c_str());
	return TRUE;
}

//
// GetAttribute: These versions leave the data unchanged
//
tBOOL	sgxXMLNode::GetAttribute(const sgxString &name, sgxString &value) const
{
	if (!GetAttributeImpl(name, value)) {
		return FALSE;
	}

	return TRUE;
}

tBOOL	sgxXMLNode::GetAttribute(const sgxString &name, tUINT32 &value) const
{
	sgxString str;

	if (!GetAttributeImpl(name, str)) {
		return FALSE;
	}

	value = atoi(str.c_str());
	return TRUE;
}

tBOOL	sgxXMLNode::GetAttribute(const sgxString &name, tINT32 &value) const
{
	sgxString str;

	if (!GetAttributeImpl(name, str)) {
		return FALSE;
	}

	value = atoi(str.c_str());
	return TRUE;
}

tBOOL	sgxXMLNode::GetAttribute(const sgxString &name, tREAL32 &value) const
{
	sgxString str;

	if (!GetAttributeImpl(name, str)) {
		return FALSE;
	}

	value = (tREAL32)atof(str.c_str());
	return TRUE;
}
