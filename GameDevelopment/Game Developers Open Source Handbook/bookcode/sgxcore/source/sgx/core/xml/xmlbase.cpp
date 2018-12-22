#include "sgx/core/core.hpp"
#include "sgx/core/xml/xmlbase.hpp"

sgxString sgxXMLBase::GetTab(const int tab)
{
	sgxString s;
	int n = tab;

	while(n--) {
		s += "   ";
	}

	return s;
}

void sgxXMLBase::Validate(sgxString &out_text, const sgxString &in_text)
{
	// TODO:
	// < &lt;
	// > &rt;
	// & &amp;
	// " &quot;
	// ' &apos;

	out_text = in_text;
}
