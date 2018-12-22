#ifndef WBA_ARGUMENT_H
#define WBA_ARGUMENT_H

/******************************************************************************

Coral Tree wba/argument.h
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

#include <gpl/gpl.h>

/***********************************************************************
class WBA_Argument
***********************************************************************/
class WBA_Argument
{
	public:
		WBA_Argument(void);
		WBA_Argument(const GPL_String &k);
		WBA_Argument(const GPL_String &k, const GPL_String &v);
		WBA_Argument(const GPL_String &k, const GPL_String &v,
						const GPL_String &d);
		WBA_Argument(const WBA_Argument &arg);
		virtual ~WBA_Argument(void);

		WBA_Argument & operator=(const WBA_Argument &arg);
		GPL_Boolean operator==(const WBA_Argument &arg) const;

		GPL_Boolean GetQuiet(void) { return quiet; }
		void SetQuiet(GPL_Boolean q) { quiet = q; }
		void SetKey(const GPL_String &k) { key = k; }
		void SetValue(const GPL_String &v) { value = v; }
		void SetDescription(const GPL_String &d) { description = d; }
		void AddAlias(const GPL_String &a) { aliases.Append(a); }

		const GPL_String &GetKey(void) const { return key; }
		const GPL_String &GetValue(void) const { return value; }
		const GPL_String &GetDescription(void) const { return description; }
		const GPL_List<GPL_String> &GetAliases(void) const { return aliases; }

		GPL_Boolean IsAlias(const GPL_String &alias) const;
		GPL_Boolean IsTrue(void) const;
		GPL_Boolean IsFalse(void) const;

		long GetLong(void) const;
		double GetDouble(void) const;
		GPL_Array<GPL_String> GetValues(const GPL_String &delimiter) const;

		void SetLong(long l);
		void SetDouble(double f);
		void SetValues(const GPL_Array<GPL_String> &values,
						const GPL_String &delimiter);

		void Increment(void);
		void Decrement(void);

		void Print(void) const;
		
	private:
		GPL_String				key;
		GPL_String				value;
		GPL_String				description;
		GPL_List<GPL_String>	aliases;
		GPL_Boolean				quiet;
};

class WBA_ArgumentTable : public GPL_HashTable<WBA_Argument, GPL_String>
{
	public:
		WBA_ArgumentTable(long a_table_size)
			: GPL_HashTable<WBA_Argument, GPL_String>(a_table_size){}
		~WBA_ArgumentTable(void);
		long HashFunction(void *element)
		{
			GPL_String *string;
			string = (GPL_String *)element;
			return string->GetLength() % GetSize();
		}
		long Compare(void *node, void *element);
		void SetQuiet(const GPL_String &key);
		void SetQuiet(const GPL_String &key, const GPL_String &value, const GPL_String &descrip, long alias_cnt, ...); 
		void Set(const GPL_String &key);
		void Set(const GPL_String &key, const GPL_String &value, const GPL_String &descrip, long alias_cnt, ...); 
		WBA_Argument *Find(const char *key)
		{
			GPL_String tmp;
			tmp = key;
			return Find(tmp);
		}
		WBA_Argument *Find(const GPL_String &key)
		{
			WBA_Argument *arg;
			GPL_String tmpkey;
			tmpkey = key;
			arg = LookUp(&tmpkey);
			if(arg) return arg;
			GPL_HashTableIterator iterator;
			iterator.Reset(this);
			while((arg=(WBA_Argument *)iterator.Next()))
			{
				if(arg->IsAlias(key)) break;
			}
			return arg;
		}
		char **BuildEnvp();
		void FreeEnvp(char **envp);
		void Print(void)
		{
			WBA_Argument *arg;
			GPL_HashTableIterator iterator;
			iterator.Reset(this);
			while((arg=(WBA_Argument *)iterator.Next()))
			{
				arg->Print();
			}
		}
	public:
		void Set(WBA_Argument *arg)
		{
			GPL_String k;
			WBA_Argument *tmp;
			k = arg->GetKey();
			tmp = LookUp(&k); // want to have only one definition at a time
			if(tmp) 
			{
				Remove(&k);
				delete tmp;
			}
			Insert(arg, &k);
		}
};

void wbaFreeEnvpStruct(char **envp);

#endif /* WBA_ARGUMENT_H */
