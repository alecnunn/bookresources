using System;
using System.Collections.Generic;

namespace CYK
{
	class Class
	{
        static List<string> Grammar = new List<string>();      // grammar rules

		static void ReadGrammar()
        {
            string s;

			while((s = Console.In.ReadLine()) != "") 
                Grammar.Add(s); 
        }

        static List<char> FindNonterminalsFor(char a)  // find a rule of a kind A -> a
        {
            List<char> result = new List<char>();

            foreach(string rule in Grammar)
                if(rule.Length == 3 && rule[2] == a)
                    result.Add(rule[0]);

            return result;
        }

        // find rules of a kind A -> BC, where B is from BSet, and C is from CSet
        static List<char> FindNonterminalsFor(List<char> BSet, List<char> CSet)
        {
            List<char> result = new List<char>();

            foreach(string rule in Grammar)
                if(rule.Length == 4 && BSet.Contains(rule[2]) && CSet.Contains(rule[3]))
                    result.Add(rule[0]);

            return result;
        }

        static void Main(string[] args)
		{
            ReadGrammar();
            string input = Console.ReadLine();      // input string 
            int N = input.Length;

            List<char>[,] V = new List<char>[N + 1, N + 1];  // table V

            for(int i = 1; i <= N; i++)                    // fill the left table 
                V[i, 1] = FindNonterminalsFor(input[i - 1]);

            for(int j = 2; j <= N; j++)                    // main part
                for(int i = 1; i <= N - j + 1; i++)
                {
                    V[i, j] = new List<char>();             // V[i, j] = {}
                    for(int k = 1; k <= j - 1; k++)
                        V[i, j].AddRange(FindNonterminalsFor(V[i, k], V[i + k, j - k]));
                }

            Console.WriteLine("The string belongs to the language: " + V[1, N].Contains('S'));
 		}
	}
}
