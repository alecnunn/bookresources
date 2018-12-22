using System;
using System.Collections.Generic;


namespace CNF
{
	class Class
	{
        static List<string> Grammar = new List<string>(); // grammar rules
        static string Terminals = "";                          // the list of terminals
        static string Nonterminals = "";                       // the list of nonterminals
        static bool AcceptEmptyString;                    // should we accept an empty string?
        static string FreeNonterminals = "";

        static void ReadGrammar()
        {
            string s;

			while((s = Console.ReadLine()) != "")   // read rules
            {
                Grammar.Add(s);                 // add a rule to the grammar

                foreach(char c in s)           // analyze rule elements
                    if(c != ' ')
                    {
                        // if the current symbol is a terminal, not yet added to Terminals
                        if(c == Char.ToLower(c) && Terminals.IndexOf(c) == -1 && c != '~')
                            Terminals += c;

                        // analogously for nonterminals
                        if(c != Char.ToLower(c) && Nonterminals.IndexOf(c) == -1)
                            Nonterminals += c;
                    }
            }
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
                        // accept an empty string, if the left side of the rule contains starting symbol
                        if(rule[0] == 'S') 
                            AcceptEmptyString = true;

                        // add all new rules for nonterminal A
                        AddRulesWithoutA(rule[0]);
                        Grammar.Remove(rule);  // remove epsilon-rule
                        EpsilonRulesExists = true;
                        break;
                    }
            }
            while(EpsilonRulesExists);   // while there are epsilon-rules
        }
        
        static bool IsAtoB(string rule)
        {
            return rule.Length == 3 && Nonterminals.IndexOf(rule[2]) != -1;
        }
        
        static bool CanObtain(char A, char B) // obtain B from A
        {
            foreach(string rule in Grammar)
                if(rule[0] == A && IsAtoB(rule)) // A -> X
                {
                    if(rule[2] == B || CanObtain(rule[2], B))
                        return true;
                }
            return false;
        }
  
        static void RemoveAtoBRules()
        {
            List<string> NewGrammar = new List<string>();
            List<string> NewRules = new List<string>();
            
            foreach(string rule in Grammar)
                if(!IsAtoB(rule))
                    NewGrammar.Add(rule);
            
            foreach(char A in Nonterminals)
                foreach(char B in Nonterminals)
                    if(CanObtain(A, B))
                    {
                        foreach(string rule in NewGrammar)
                            if(rule[0] == B)
                                NewRules.Add(A + rule.Substring(1));
                    }
            
            NewGrammar.AddRange(NewRules);
            Grammar = NewGrammar;
        }
        
        static void FindFreeNonterminals()
        {
            for(char c = 'A'; c <= 'Z'; ++c)
                if(Nonterminals.IndexOf(c) == -1)
                    FreeNonterminals += c;
        }

        static char GetNewVar(List<string> newrules, char OldVar)  // get a symbol of a new rule
        {                                                          // having a symbol of an old rule
            char NewVar;

            if(Nonterminals.IndexOf(OldVar) != -1)   // if the symbol is a nonterminal
                NewVar = OldVar;                     // it goes to the new rule without changes
            else                                     
            {
                NewVar = FreeNonterminals[0];                     // if the symbol is a terminal
                FreeNonterminals = FreeNonterminals.Substring(1);
                newrules.Add(NewVar + " " + OldVar); // we should add a rule of kind X -> a
            }                                        // and return a new nonterminal X

            return NewVar;
        }

        static void ConvertRules()
        {
            List<string> ToDel = new List<string>();    // the list of rules to be deleted
            List<string> newrules = new List<string>();
            
            // for each rule of a kind A -> alpha, where length(alpha) > 1
            foreach(string rule in Grammar)
                if(rule.Length > 3)
                {
                    char L = rule[0];  // left and right sides of the rule
                    string alpha = rule.Substring(2);

                    ToDel.Add(rule);   // put a rule into the list of rules to be deleted
                    
                    while(alpha.Length >= 2)   // while the rule is not completely parsed
                    {
                        // generate a new rule of a kind L -> XY
                        // first we get X (having alpha[0])
                        char X = GetNewVar(newrules, alpha[0]);

                        // if the initial rule contains just two symbols, 
                        // we obtain Y value by calling GetNewVar()
                        // otherwise we should take a new nonterminal from the list of nonterminals
                        char Y = (alpha.Length == 2) ? GetNewVar(newrules, alpha[1]) : FreeNonterminals[0];

                        // add the rule L -> XY
                        newrules.Add(L + " " + X + Y);
                        alpha = alpha.Substring(1); // shorten the initial rule
                        L = FreeNonterminals[0];
                        FreeNonterminals = FreeNonterminals.Substring(1); // take a new nonterminal from the list
                    }
                }

            foreach(string rule in ToDel)
                Grammar.Remove(rule);

            Grammar.AddRange(newrules);
        }

        static void Main(string[] args)
		{
            ReadGrammar();
            RemoveEpsilonRules();
            RemoveAtoBRules();
            FindFreeNonterminals();
            ConvertRules();
            
            Console.WriteLine("Rules:");
            foreach(string rule in Grammar)
                Console.WriteLine(rule);
            Console.WriteLine("Accept empty string: " + AcceptEmptyString); 
		}
	}
}
