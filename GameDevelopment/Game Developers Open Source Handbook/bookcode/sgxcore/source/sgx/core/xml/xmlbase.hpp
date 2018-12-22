#ifndef SGX_CORE_XML_XMLBASE_HPP
#define SGX_CORE_XML_XMLBASE_HPP	1

//
// Raw functionality for all XML features
//
class sgxXMLBase {
public:
	static void			Validate(sgxString &out, const sgxString &text);
	static sgxString	GetTab(int tab);
};

//
// Features of an XML tag
//
class sgxXMLAttribute : public sgxXMLBase {
public:
	sgxString		m_Name;
	sgxString		m_Value;	

	sgxXMLAttribute(const sgxString &name, const sgxString &value) 
	{
		m_Name = name;
		m_Value = value;
	}
};

class sgxXMLElement : public sgxXMLBase {
public:
	sgxXMLElement() : m_Name("") { }
	sgxXMLElement(const char *name) : m_Name(name) { }
	sgxXMLElement(const sgxString &name) : m_Name(name) { }

	sgxString		m_Name;
};

//
// Nodes of a DOM tree
//
class sgxXMLNode : public sgxXMLBase {
public:
	sgxXMLElement					m_Element;
	sgxVector<sgxXMLAttribute>		m_Attribute;
	sgxString						m_Data;
	sgxVector<sgxXMLNode *>			m_ChildNodes;

	sgxXMLNode() {}
	~sgxXMLNode();

	sgxXMLNode *GetChild(const sgxString &child);
	const sgxXMLNode *GetChild(const sgxString &child) const;


	void	SetData(const sgxString &data)				{ m_Data = data; }
	void	GetData(sgxString &data)					{ data = m_Data; }
	void	AppendData(const sgxString &data)			{ m_Data += data; }
	void	SetElement(const sgxXMLElement &ele)		{ m_Element = ele; }
	sgxXMLNode *AddNode(sgxXMLNode *pNode)				{ m_ChildNodes.push_back(pNode); return m_ChildNodes.back(); }
	void	AddAttribute(const sgxXMLAttribute &attr)	{ m_Attribute.push_back(attr); }
	void	AddAttribute(const sgxString &name, const sgxString &value)	{ m_Attribute.push_back(sgxXMLAttribute(name,value)); }
	tBOOL	DoesAttributeExist(const sgxString &name) const;
	tBOOL	GetAttribute(const sgxString &name, sgxString &value) const;
	tBOOL	GetAttribute(const sgxString &name, tINT32 &value) const;
	tBOOL	GetAttribute(const sgxString &name, tUINT32 &value) const;
	tBOOL	GetAttribute(const sgxString &name, tREAL32 &value) const;
	tBOOL	GetAttribute(const sgxString &name, sgxString &value, const sgxString &default_string) const;
	tBOOL	GetAttribute(const sgxString &name, tINT32 &value, const tINT32 default_value) const;
	tBOOL	GetAttribute(const sgxString &name, tUINT32 &value, const tUINT32 default_value) const;
	tBOOL	GetAttribute(const sgxString &name, tREAL32 &value, const tREAL32 default_value) const;

private:
	tBOOL	GetAttributeImpl(const sgxString &name, sgxString &value) const;

};

#endif /* SGX_CORE_XML_XMLBASE_HPP */
