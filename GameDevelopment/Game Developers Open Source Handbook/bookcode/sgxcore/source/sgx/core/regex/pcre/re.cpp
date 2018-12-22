#include "re.h"

unsigned
Regexp::options()
{
	return _opts;
}

void					
Regexp::options(unsigned opts)
{
	_opts = opts;
}

void 
Regexp::compile(const string & s)
{
	const char * errorptr;
	int erroroffset;
	
	re = pcre_compile(s.c_str(), _opts, &errorptr, &erroroffset, 0);
	if ( !re )
		throw exception(string(errorptr) + " at: " + s.substr(erroroffset));
}

pcre *
Regexp::clone_re(pcre * re)
{
	if ( !re )
		return 0;
	size_t size;
	pcre_fullinfo(re, 0, PCRE_INFO_SIZE, &size);
	pcre * newre = (pcre *) new char[size];
	if ( !newre )
		throw exception("not enough memory");
	memcpy(newre, re, size);
	return newre;
}

Regexp::Regexp(unsigned opts) 
{
	_opts = opts;
	re = 0;
}

Regexp::Regexp(const string & s, unsigned opts)
{
	_opts = opts;
	compile(s);
}

const Regexp & 
Regexp::operator = (const string & s)
{
	if ( re )
		delete[] re;
	compile(s);
	return *this;
}

Regexp::Regexp(const char * s, unsigned opts)
{
	_opts = opts;
	compile(s);
}

const Regexp & 
Regexp::operator = (const char * s)
{
	if ( re )
		delete[] re;
	compile(s);
	return *this;
}

Regexp::Regexp(const Regexp & r)
{
	_opts = r._opts;
	re = clone_re(r.re);
}

const Regexp & 
Regexp::operator = (const Regexp & r)
{
	_opts = r._opts;
	if ( re )
		delete[] re;
	re = clone_re(r.re);
	return *this;
}

Regexp::~Regexp()
{
	if ( re )
		delete[] re;
}

vector<Regexp::markers> 
Regexp::match(const string & s, unsigned offset) const
{
	if ( !re )
		throw exception("match on unitialized expression");

	size_t msize;
	pcre_fullinfo(re, 0, PCRE_INFO_CAPTURECOUNT, &msize);
	msize = 3*(msize+1);
	int *m = new int[msize];
	if ( !m )
		throw exception("not enough memory");
	int result;
	vector<markers> marks;

	result = pcre_exec(re, 0, s.c_str(), s.length(), offset, 0, m, msize);
	for ( int i = 0, *p = m ; i < result ; i++, p+=2 )
		marks.push_back(markers(p[0], p[1]));
	delete[] m;
	return marks;
}

vector<Regexp::markers>
Regexp::gmatch(const string & s) const
{
	if ( !re )
		throw exception("gmatch on unitialized expression");

	int m[3];
	vector<markers> marks;

	const char * str = s.c_str();
	unsigned offset = 0, len = s.length();
	while ( offset < len && pcre_exec(re, 0, str, len, offset, 0, m, 3) >= 0 )
	{
		marks.push_back(markers(m[0], m[1]));
		offset = m[1];
	}
	return marks;
}

string
Regexp::substr(const string & s, const vector<Regexp::markers> & marks,
															unsigned index)
{
	if ( index >= marks.size() )
		throw exception("bad substring index");

	int begin = marks[index].first;
	if ( begin == -1 )
		return "";
	int end = marks[index].second;
	return s.substr(begin, end-begin);
}

vector<string>
Regexp::substr(const string & s, const vector<Regexp::markers> & marks)
{
	vector<string> v;
	unsigned size = marks.size();

	for ( unsigned i = 0 ; i < size ; i++ )
		v.push_back(substr(s, marks, i));
	return v;
}

vector<string>
Regexp::split(const string & s, bool emptyfields, unsigned maxfields) const
{
	vector<markers> m = gmatch(s);
	vector<markers> marks;

	int begin = 0, end;
	for ( int i = 0, nsep = m.size() ; i < nsep ; i++ )
	{
		end = m[i].first;
		if ( emptyfields || end > begin )
			marks.push_back(markers(begin, end));
		begin = m[i].second;
	}
	end = s.length();
	if ( emptyfields || end > begin )
		marks.push_back(markers(begin, end));
	unsigned nfields = marks.size();
	if ( maxfields && nfields > maxfields )
	{
		marks[maxfields-1].second = marks[nfields-1].second;
		// SGX_EDIT
		int numRemove = nfields-maxfields;
		while(numRemove--) {
			marks.erase(marks.end());
		}
		// SGX_EDIT END
	}
	return substr(s, marks);
}

vector<string>
Regexp::split(const string & s, unsigned maxfields, bool emptyfields) const
{
	return split(s, emptyfields, maxfields);
}
