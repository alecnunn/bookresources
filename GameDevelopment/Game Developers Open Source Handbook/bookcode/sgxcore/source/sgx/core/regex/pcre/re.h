#ifndef RE_H
#define RE_H

#include "sgx/core/stl/stl.hpp"

using namespace std;

#include "pcre.h"

class Regexp
{
public:

	class exception : public runtime_error
	{
	public:
		exception(const string & msg) : runtime_error(msg) { }
	};

	enum // bits de opciones
	{
		anchored =	PCRE_ANCHORED,
		caseless =	PCRE_CASELESS,
		dollarend =	PCRE_DOLLAR_ENDONLY,
		dotall =	PCRE_DOTALL,
		extended =	PCRE_EXTENDED,
		multiline =	PCRE_MULTILINE,
		ungreedy =	PCRE_UNGREEDY
	};

	Regexp(unsigned opts = 0);
	Regexp(const string & s, unsigned opts = 0);
	const Regexp & operator = (const string & s);
	Regexp(const char * s, unsigned opts = 0);
	const Regexp & operator = (const char * s);
	Regexp(const Regexp & r);
	const Regexp & operator = (const Regexp & r);
	~Regexp();

	typedef pair<int, int> markers;

	unsigned				options();
	void					options(unsigned opts);
	vector<markers>			match(const string & s, unsigned offset = 0) const;
	vector<markers>			gmatch(const string & s) const;
	static string			substr(const string & s,
								const vector<markers> & marks, unsigned index);
	static vector<string>	substr(const string & s,
								const vector<markers> & marks);
	vector<string>			split(const string & s, bool emptyfields = true,
								unsigned maxfields = 0) const;
	vector<string>			split(const string & s, unsigned maxfields,
								bool emptyfields = true) const;

private:
	pcre * re;
	unsigned _opts;

	void compile(const string & s);
	static pcre * clone_re(pcre * re);

	friend class sgxRegex;
};

#endif
