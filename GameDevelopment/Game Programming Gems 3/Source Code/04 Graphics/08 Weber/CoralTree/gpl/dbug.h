
/******************************************************************************

Coral Tree gpl/dbug.h
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

#if GPL_VERBOSITY==GPL_DEBUG
#define DB(a) 			GPL_Debug gpl_dbug(a, __FILE__, __LINE__);
#define DP(a) 			gpl_dbug.print(a, __FILE__, __LINE__);
#define DH()  			gpl_dbug.header(__FILE__, __LINE__);
#define DPF 			gpl_dbug.header(__FILE__, __LINE__);fprintf
#else
#define DB(a) 			/* a */
#define DP(a) 			/* a */
#define DH()  			/* */
#define DPF   			//
#endif


/******************************************************************************
By placing a debug object within each function or method in a program, the
line of execution can be traced.  The debugging output is formatted so that
each level into the debug execution stack is indented.  By using the debug
print method, extra debugging information can be properly indented as well.

The above macros make code using this debug object easier to read and more
flexible (can be turn off, etc.)

TODO: make a preprocessor to embed DB calls automatically.

keywords: c++, debug
******************************************************************************/
class GPL_Debug
{
	public:

static	int		Depth;
static	char	Buff[100];
		char	*Name;

				GPL_Debug(char *, char *, int);
virtual			~GPL_Debug();

		void	print(char *, char *, int);
		void	header(char *, int);
};

class GPL_Timer
{
	private:
		unsigned long t, last;
		GPL_String    s;
		static int nextid;
		int id;
	public:
		GPL_Timer(const GPL_String &str)
		{
			s = str;
			t = osdTimer();
			last = t;
			id = nextid++;
		}
virtual	~GPL_Timer(){}

		void Print(const GPL_String &str)
		{
			unsigned long now;
			now = osdTimer();
			fprintf(stderr,"%s : %d : %s %u %u\n",
				(const char *)(const GPL_String)s, id, 
				(const char *)str, (unsigned int)(now - t), 
				(unsigned int)(now - last));
			last = now;
		}
};


