using System;

namespace LL_Parser
{
	class Class
	{
        static string input;        // input string

        static char NextChar()      // the next symbol of the input string
        {
            return input[0];
        }

        static void AdvancePointer()     // advance to the next symbol
        {
            input = input.Substring(1);
        }

        static void S()                     // S -> E
        {
            Console.WriteLine("S -> E");
            E();
        }

        static void E()                     // E -> TF
        {
            Console.WriteLine("E -> TF");
            T(); F();
        }

        static void F()                    // F -> +E | *E | ~
        {
            string rule;

            if(NextChar() == '+')       rule = "+E";  // choose the alternative
            else if(NextChar() == '*')  rule = "*E";
            else                        rule = "~";

            Console.WriteLine("F -> " + rule);
            switch(rule)
            {
                case "+E": AdvancePointer(); E(); break;  // F -> +E
                case "*E": AdvancePointer(); E(); break;  // F -> *E
                case  "~": break;                         // F -> ~
            }
        }

        static void T()                    // T -> a | b | c | (E)
        {
            string rule;

            if(NextChar() == 'a')       rule = "a";     // choose the alternative
            else if(NextChar() == 'b')  rule = "b";
            else if(NextChar() == 'c')  rule = "c";
            else if(NextChar() == '(')  rule = "(E)";
            else                        throw new Exception();

            Console.WriteLine("T -> " + rule);
            switch(rule)
            {
                case   "a": AdvancePointer(); break;      // T -> a
                case   "b": AdvancePointer(); break;      // T -> b
                case   "c": AdvancePointer(); break;      // T -> c
                case "(E)": AdvancePointer();             // T -> (E)
                            E();
                            if(NextChar() != ')')  throw new Exception();
                            AdvancePointer();
                            break;
            }
        }

		static bool Parse()
        {
            try
            {
                S();
                if(NextChar() == '$')
                    return true;
            }
            catch(Exception)
            {
            }

            return false;
        }
        
        static void Main(string[] args)
		{
            input = Console.ReadLine() + "$";
            Console.WriteLine(Parse() ? "String accepted" : "String rejected");
		}
	}
}
