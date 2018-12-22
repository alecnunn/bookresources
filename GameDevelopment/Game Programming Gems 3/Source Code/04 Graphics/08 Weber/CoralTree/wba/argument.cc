/******************************************************************************

Coral Tree wba/argument.cc
Copyright (C) 1998 Infinite Monkeys Incorporated

This program is free software; you can redistribute it and/or modify
it under the terms of the Artistic License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
Artistic License for more details.

You should have received a copy of the Artistic License along with
this program (see meta/ART_LICE); if not, write to coral@imonk.com.

******************************************************************************/

#include <wba/wba.h>

/***********************************************************************
class WBA_Argument
***********************************************************************/
WBA_Argument::WBA_Argument(void)
{
	SetKey("");
	SetLong(0);
	SetQuiet(TRUE);
	SetDescription("");
}

WBA_Argument::WBA_Argument(const GPL_String &k)
{
	SetKey(k);
	SetLong(0);
	SetQuiet(TRUE);
	SetDescription("");
}

WBA_Argument::WBA_Argument(const GPL_String &k, const GPL_String &v)
{
	SetKey(k);
	SetValue(v);
	SetQuiet(TRUE);
	SetDescription("");
}

WBA_Argument::WBA_Argument(const GPL_String &k, const GPL_String &v, const GPL_String &d)
{
	SetKey(k);
	SetValue(v);
	SetQuiet(TRUE);
	SetDescription(d);
}

WBA_Argument::WBA_Argument(const WBA_Argument &arg)
	: key(arg.key), value(arg.value), description(arg.description),
	aliases(arg.aliases), quiet(arg.quiet)
{
}

WBA_Argument::~WBA_Argument(void)
{
}

WBA_Argument & WBA_Argument::operator=(const WBA_Argument & arg)
{
	key = arg.key;
	value = arg.value;
	description = arg.description;
	aliases = arg.aliases;
	quiet = arg.quiet;
	return *this;
}

GPL_Boolean WBA_Argument::operator==(const WBA_Argument & arg) const
{
	return key == arg.key && value == arg.value &&
		description == arg.description && aliases == arg.aliases &&
		quiet == arg.quiet;
}

GPL_Boolean WBA_Argument::IsAlias(const GPL_String &alias) const
{
	return aliases.Contains(alias);
}

GPL_Boolean WBA_Argument::IsTrue(void) const
{
	return !IsFalse();
}

GPL_Boolean WBA_Argument::IsFalse(void) const
{
	return (	value.IsNull() 		|| 
				value == "0" 		|| 
				value == "FALSE" 	||
				value == "False" 	|| 
				value == "false"	);
}

long WBA_Argument::GetLong(void) const
{
	if (value.IsNull())
	{
		return 0;
	}
	return atol(value.GetData());
}

double WBA_Argument::GetDouble(void) const
{
	if (value.IsNull())
	{
		return 0.0;
	}
	return atof(value.GetData());
}

/***********************************************************************
GPL_Array<GPL_String> WBA_Argument::GetValues(const GPL_String &delimiter) const
***********************************************************************/
GPL_Array<GPL_String> WBA_Argument::GetValues(const GPL_String &delimiter) const
{
	GPL_Array<GPL_String> values;
	values.Resize(4); // Arbitrary guess
	GPL_StringTokenizer tokens;
	tokens.Reset(GetValue(), delimiter);
	for (GPL_String t = tokens.Next(); (char *) 0 != t.GetData(); t = tokens.Next())
	{
		values.AppendElement(t);
	}
	return values;
}

void WBA_Argument::SetLong(long l)
{
	GPL_String tmp;
	tmp.Printf("%d", l);
	value = tmp;
}

void WBA_Argument::SetDouble(double f)
{
	GPL_String tmp;
	tmp.Printf("%f", f);
	value = tmp;
}

/***********************************************************************
void WBA_Argument::SetValues(const GPL_Array<GPL_String> &values,
								const GPL_String &delimiter)
***********************************************************************/
void WBA_Argument::SetValues(const GPL_Array<GPL_String> &values,
								const GPL_String &delimiter)
{
	SetValue(GPL_String::Join(delimiter, values));
}

void WBA_Argument::Increment(void)
{
	long l;
	l = GetLong();
	l++;
	SetLong(l);
}

void WBA_Argument::Decrement(void)
{
	long l;
	l = GetLong();
	l--;
	SetLong(l);
}

void WBA_Argument::Print(void) const
{
	GPL_String v = value;
	if (value.IsNull())
	{
		v = "NULL";
	}
	GPL_String d = description;
	if (description.IsNull())
	{
		d = "no description";
	}

	if ((WBA_Application *) GPL_BAD_ADDRESS != WBA_APP)
	{
		WBA_APP->Printf("[%s] [%s] [%s]", key.GetData(),
						v.GetData(), d.GetData());
		register const unsigned long n = aliases.GetCount();
		for (register unsigned long i = 0; n > i; ++i)
		{
			WBA_APP->Printf("     alias [%s]", aliases[i].GetData());
		}
	}
}

WBA_ArgumentTable::~WBA_ArgumentTable(void)
{
	/* clean out all the arguments */
	WBA_Argument *arg;
	GPL_HashTableIterator iterator;
	iterator.Reset(this);
	while((arg=(WBA_Argument *)iterator.Next()))
	{
		delete arg;
	}
}

void WBA_ArgumentTable::Set(const GPL_String &key)
{
	Set(key, (char *)NULL, (char *)NULL, 0);
}

void WBA_ArgumentTable::SetQuiet(const GPL_String &key)
{
	SetQuiet(key, (char *)NULL, (char *)NULL, 0);
}


/* TODO: redo Set and SetQuiet so as to not be cut-and-pasted */
void WBA_ArgumentTable::Set(const GPL_String &key, const GPL_String &value, const GPL_String &descrip, long alias_cnt, ...)
{
	WBA_Argument *arg;
	arg = Find(key);
	if(!arg)
	{
		arg = new WBA_Argument(key);
		Set(arg);
	}
	arg->SetValue(value);
	arg->SetDescription(descrip);
	arg->SetQuiet(FALSE);

	va_list ap;
	va_start(ap, alias_cnt);
	while(alias_cnt > 0)
	{
		arg->AddAlias(va_arg(ap, char *));
		alias_cnt--;
	}
	va_end(ap);
}

void WBA_ArgumentTable::SetQuiet(const GPL_String &key, const GPL_String &value, const GPL_String &descrip, long alias_cnt, ...)
{
	WBA_Argument *arg;
	arg = Find(key);
	if(!arg)
	{
		arg = new WBA_Argument(key);
		Set(arg);
	}
	arg->SetValue(value);
	arg->SetDescription(descrip);

	va_list ap;
	va_start(ap, alias_cnt);
	while(alias_cnt > 0)
	{
		arg->AddAlias(va_arg(ap, char *));
		alias_cnt--;
	}
	va_end(ap);
}

long WBA_ArgumentTable::Compare(void *node, void *element)
{
	GPL_String *string;
	WBA_Argument *arg;
	string = (GPL_String *)element;
	arg = (WBA_Argument *)node;

	if(*string == arg->GetKey()) return 0;

	return 1;	
}

char **WBA_ArgumentTable::BuildEnvp()
{
	char **return_val;
	WBA_Argument *arg;
	long i = 0;
	return_val = (char **)osdMalloc((1+GetCount())*sizeof(char **));
	return_val[GetCount()] = NULL;
	GPL_HashTableIterator iterator;
	iterator.Reset(this);
	while((arg = (WBA_Argument *)iterator.Next()))
	{
		GPL_String sarg;
		sarg.Printf("%s=%s",(const char *)(arg->GetKey()),(const char *)(arg->GetValue()));
		return_val[i] = osdSPrintf("%s", (char *) sarg);
		i++;
	}
	return return_val;
}

void WBA_ArgumentTable::FreeEnvp(char **envp)
{
	wbaFreeEnvpStruct(envp);
}

void wbaFreeEnvpStruct(char **envp)
{
	if(!envp) return;
	long i;
	i = 0;
	while(envp[i])
	{
		osdFree(envp[i]);
		i++;
	}
	osdFree(envp);
}

