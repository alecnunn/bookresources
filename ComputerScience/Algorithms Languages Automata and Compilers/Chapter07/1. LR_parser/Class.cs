using System;
using System.Collections.Generic;

namespace LR_DPDA
{
	class Class
	{
        static List<string> Grammar = new List<string>(); // grammar rules
        static string Terminals = "";                          // list of terminals
        static string Nonterminals = "";                       // list of nonterminals
        static bool AcceptEmptyString;                    // should we accept an empty string?
         
        static Dictionary<TableKey, string> ACTION;
        static Dictionary<TableKey, int> GOTO;

        static Dictionary<char, string> FirstSets = new Dictionary<char, string>();  // a collection of First sets

        static void ReadGrammar()
        {
            string s;

			while((s = Console.ReadLine()) != "")   // read rules
            {
                Grammar.Add(s);                 // add a rule to the grammar

                foreach(char c in s)           // analyze rule elements
                    if(c != ' ')
                    {
                        // if a current symbol is a terminal, not yet added to Terminals
                        if(c == Char.ToLower(c) && Terminals.IndexOf(c) == -1 && c != '~')
                            Terminals += c;

                        // analogously for nonterminals
                        if(c != Char.ToLower(c) && Nonterminals.IndexOf(c) == -1)
                            Nonterminals += c;
                    }
            }
        }

        struct TableKey   // key of ACTION and GOTO tables
        {
            public int I;
            public char J;

            public TableKey(int i, char j) { I = i; J = j; }
        }

        static TableKey MakeTableKey(int i, char j)
        {
            return new TableKey(i, j);
        }
    
        static void GenerateRulesWithoutA(List<string> result, char A, string prefix, string suffix)
        {
            int A_idx = suffix.IndexOf(A);
            if(A_idx == -1)
            {
                string rule = prefix + suffix;
                if(rule.Length < 3)
                    rule += "~";
                result.Add(rule);
            }
            else
            {
                GenerateRulesWithoutA(result, A, prefix + suffix.Substring(0, A_idx + 1), suffix.Substring(A_idx + 1));
                GenerateRulesWithoutA(result, A, prefix + suffix.Substring(0, A_idx), suffix.Substring(A_idx + 1));
            }
                
        }        
        
        static void AddRulesWithoutA(char A)
        {
            List<string> newRules = new List<string>();
            
            foreach(string rule in Grammar)
            {
                string rhs = rule.Substring(2);
                
                if(rhs.IndexOf(A) != -1)
                    GenerateRulesWithoutA(newRules, A, rule[0] + " ", rhs);
            }
            
            foreach(string rule in newRules)
                if(!Grammar.Contains(rule))
                    Grammar.Add(rule);
        }

        static void RemoveEpsilonRules()
        {
            AcceptEmptyString = false;
            bool EpsilonRulesExists;

            do
            {
                EpsilonRulesExists = false;
                foreach(string rule in Grammar)
                    if(rule[2] == '~')  // epsilon-rule found
                    {
                        // accept empty string if the left side of the rule
                        // contains starting symbol
                        if(rule[0] == 'S') 
                            AcceptEmptyString = true;

                        // add new rules for nonterminal A
                        AddRulesWithoutA(rule[0]);
                        Grammar.Remove(rule);  // remove epsilon-rule
                        EpsilonRulesExists = true;
                        break;
                    }
            }
            while(EpsilonRulesExists);   // while there are epsilon-rules
        }

        static void ComputeFirstSets()
        {
            foreach(char c in Terminals)  // FIRST[c] = {c}
                FirstSets[c] = c.ToString();

            foreach(char X in Nonterminals) // FIRST[X] = {}
                FirstSets[X] = "";
            
            bool changes;
            do
            {
                changes  = false;

                foreach(string rule in Grammar)
                {
                    // for each rule X -> Y0Y1...Yn
                    char X = rule[0];
                    char Y0 = rule[2];

                    foreach(char a in Terminals)
                        if(FirstSets[Y0].IndexOf(a) != -1 && FirstSets[X].IndexOf(a) == -1)
                        {
                            FirstSets[X] += a;   // add a to FirstSets[X]
                            changes = true;
                        }
                }
            }
            while(changes);  // while there are changes
        }
        
        static string First(char X)
        {
            return FirstSets[X];
        }

        static string First(string X)
        {
            return First(X[0]);
        }

        static List<string> Closure(List<string> I)
        {
            List<string> R = new List<string>();

            R.AddRange(I);    // add all elements of I into the closure

            bool changes;
            do
            {
                changes = false;

                foreach(string r_item in R)               // for each element A -> alpha.Bbeta,a
                {
                    int Bidx = r_item.IndexOf('.') + 1;
                    char B = r_item[Bidx];                 // B

                    if(Nonterminals.IndexOf(B) == -1)
                        continue;

                    string beta = r_item.Substring(Bidx + 1);
                    beta = beta.Substring(0, beta.Length - 2);  // beta
                    char a = r_item[r_item.Length - 1];         // a

                    foreach(string rule in Grammar)   // for each rule B -> gamma
                        if(rule[0] == B)              
                        {
                            string gamma = rule.Substring(2);  // gamma

                            foreach(char b in First(beta + a)) // for each b from FIRST(betaa)
                            {
                                string newitem = B + " ." + gamma + "," + b;

                                if(!R.Contains(newitem))    // add element B -> .gamma,b
                                {
                                    R.Add(newitem);
                                    changes = true;
                                    goto breakloop;
                                }
                            }
                        }
                }
                breakloop:;
            }
            while(changes);

            return R;
        }

        static List<string> GoTo(List<string> I, char X)
        {
            List<string> J = new List<string>();

            foreach(string item in I)
            {
                string[] parts = item.Split('.');

                if(parts[1][0] != X)
                    continue;

                // if this situation is of a kind A alpha.Xbeta,a
                J.Add(parts[0] + X + "." + parts[1].Substring(1));
            }

            return Closure(J);
        }

        static bool IsSubset(List<string> lhs, List<string> rhs)
        {
            foreach(string s in lhs)
                if(!rhs.Contains(s))
                    return false;
            return true;
        }
        
        static bool SetsEqual(List<string> lhs, List<string> rhs)
        {
            return IsSubset(lhs, rhs) && IsSubset(rhs, lhs);
        }

        static bool Contains(List<List<string> > C, List<string> g)
        {
            foreach(List<string> item in C)
                if(SetsEqual(item, g))
                    return true;

            return false;
        }

        static List<List<string> > CreateC()
        {
            string Symbols = Terminals + Nonterminals;  // all grammar symbols

            List<List<string> > C = new List<List<string> >();

            // add element I0 = Closure({"> .S,$"})
            List<string> I0 = new List<string>();
            I0.Add("> .S,$");
            C.Add(Closure(I0));

            bool modified;
            do
            {
                modified = false;
                foreach(char X in Symbols) // for each grammar symbol X  
                {                                          
                    // for each element of a sequence C
                    foreach(List<string> item in C)
                    {
                        List<string> g = GoTo(item, X); // GoTo(Ii, X)
                        
                        // if g set is not empty and not yet included into C
                        if(g.Count != 0 && !Contains(C, g))
                        { 
                            C.Add(g); 
                            modified = true; 
                            break; 
                        }
                    }
                }
            }
            while(modified);   // while there are changes

            return C;
        }

        static void WriteActionTableValue(Dictionary<TableKey, string> ACTION, int I, char J, string action)
        {
            TableKey Key = MakeTableKey(I, J);

            if(ACTION.ContainsKey(Key) && !ACTION[Key].Equals(action))
                throw new Exception(); // not an LR(1) kind

            ACTION[Key] = action;
        }

        static int GotoInC(List<string> Ii, char a, List<List<string> > C)
        {
            for(int j = 0; j < C.Count; ++j)
                if(SetsEqual(GoTo(Ii, a), C[j]))
                    return j;
             return -1;
        }
        
        static Dictionary<TableKey, string> CreateActionTable(List<List<string> > C)
        {
            Dictionary<TableKey, string> ACTION = new Dictionary<TableKey, string>();

            try
            {
                for(int i = 0; i < C.Count; ++i) // for each element of of C
                {
                    List<string> Ii = C[i];  
                    
                    foreach(string item in Ii) // for each situation of a set C[i]
                    {
                        char a = item[item.IndexOf('.') + 1];  // symbol after a dot

                        // if the situation is of a kind "A alpha.abeta,b", where a is a terminal
                        // and element C[j] exists, such as GoTo(C[i], a) == C[j]
                        int j = -1; 
                        if(Terminals.IndexOf(a) != -1 && (j = GotoInC(Ii, a, C)) != -1)
                            WriteActionTableValue(ACTION, i, a, "s " + j);  // write ACTION[i, a] = shift j

                        // if the situation is of a kind "A alpha.,a" (there is a comma after a dot)
                        // and the left side is not ">"
                        if(item[item.IndexOf('.') + 1] == ',' && item[0] != '>') 
                        {
                            a = item[item.Length - 1];    // extract a
                            string alpha = item.Split('.')[0].Substring(2);  // and alpha
                            
                            // ACTION[i, a] = reduce A -> alpha
                            WriteActionTableValue(ACTION, i, a, "r " + item[0] + " " + alpha);
                        }

                        // if the situation is of a kind "> S.,$"
                        if(item.Equals("> S.,$"))
                            WriteActionTableValue(ACTION, i, '$', "a");  // ACTION[i, '$'] = accept
                    }
                }
            }
            catch(Exception) 
            { 
                return null; 
            }
            return ACTION;
        }

        static Dictionary<TableKey, int> CreateGotoTable(List<List<string> > C)
        {
            Dictionary<TableKey, int> GOTO = new Dictionary<TableKey, int>();

            foreach(char A in Nonterminals)        // for each nonterminal A
                for(int i = 0; i < C.Count; ++i)   // for each element Ii from C
                {
                    int j = GotoInC(C[i], A, C);
                    if(j != -1)
                        GOTO[MakeTableKey(i, A)] = j; // GOTO[i, A] = j
                }            
            
            return GOTO;
        }

        static void WriteGrammar()
        {
            Console.WriteLine("Rules:");
            foreach(string rule in Grammar)
                Console.WriteLine(rule);

            Console.WriteLine("Terminals: " + Terminals);
            Console.WriteLine("Nonterminals: " + Nonterminals);
            Console.WriteLine("------");
        }
            
        static bool Parse(string input)
        {
            if(input.Equals("$"))                          // the case of empty string
                return AcceptEmptyString;

            Stack<int> stack = new Stack<int>();    // stack of the automaton 
            stack.Push(0);              // add a starting (zero) state

            try
            {
                for(;;)
                {
                    int s = stack.Peek(); // top of the stack
                    char a = input[0];                               // input symbol
                    string action = ACTION[MakeTableKey(s, a)]; // ACTION table element

                    if(action[0] == 's')                   // shift
                    {
                        stack.Push(a); // push a into the stack
                        stack.Push(Convert.ToInt32(action.Substring(2))); // push s' into the stack
                        input = input.Substring(1);        // advance to the next symbol of the input string
                    }
                    else if(action[0] == 'r')              // reduce 
                    {
                        string[] rule = action.Split(' '); // rule[1] = A, rule[2] = alpha

                        for(int i = 0; i < 2 * rule[2].Length; ++i)  // remove 2 * Length(alpha)
                            stack.Pop();                             // stack elements

                        int state = stack.Peek(); 
                        // add A and GOTO[state, A] to the stack
                        stack.Push(rule[1][0]);
                        stack.Push(GOTO[MakeTableKey(state, rule[1][0])]); 

                        Console.WriteLine(rule[1] + " -> " + rule[2]);  // output rule 
                    }
                    else if(action[0] == 'a')              // accept
                        return true;
                }
            }
            catch(Exception)
            {
            }

            return false;
        }
        
        static void WriteTables()
        {
            // print the contents of ACTION and GOTO tables
            foreach(KeyValuePair<TableKey, string> item in ACTION)
                Console.WriteLine("ACTION[" + item.Key.I + ", " + item.Key.J + "] = " + item.Value);

            foreach(KeyValuePair<TableKey, int> item in GOTO)
                Console.WriteLine("GOTO[" + item.Key.I + ", " + item.Key.J + "] = " + item.Value);

            Console.WriteLine("------");
        }
        
		static void Main(string[] args)
		{
            ReadGrammar();
            RemoveEpsilonRules();
            
            Grammar.Add("> S");       // (>) complete the grammar with a rule > -> S
            Nonterminals += ">";
            Terminals += "$";         // complete the list of terminals with a dollar symbol

            WriteGrammar();

            // generate LR(1) table
            ComputeFirstSets();                 // computer FIRST sets
            List<List<string> > C = CreateC();  // create C sequence

            if((ACTION = CreateActionTable(C)) == null) // create ACTION table
            {
                Console.WriteLine("The grammar is not of LR(1) kind"); 
                return;
            }
            
            GOTO = CreateGotoTable(C);          // create GOTO table

            WriteTables();
            
            string input = Console.ReadLine() + "$";    // read input string
            Console.WriteLine(Parse(input) ? "String accepted" : "String rejected");
		}
	}
}
