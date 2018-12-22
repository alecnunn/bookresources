using System;
using System.Collections.Generic;

namespace DFAMinimization
{
	/// <summary>
	/// Summary description for Class.
	/// </summary>
	class Class
	{
		/// <summary>
		/// The main entry point for the application.
		/// </summary>

		struct Pair               // a pair of states
		{
			public int P, Q;
			public Pair(int thep, int theq) { P = thep; Q = theq; }
		}

		struct Leftside          // a left side of a rule -- a pair (state, symbol)
		{
			public int State;
			public char Symbol;
			public Leftside(int st, char sym) { State = st; Symbol = sym; }
		}

		struct DFA	
		{
			public int StatesCount;
			public char[] Symbols;
			public bool[] IsFavorable;
			public int FavorableState;
			public int StartState;
			public Dictionary<Leftside, int> Rules;
		}
		
		static DFA readInput()
		{
			DFA dfa;
			dfa.StatesCount = Convert.ToInt32(Console.In.ReadLine()); 

			string symbols_str = Console.In.ReadLine();
			dfa.Symbols = symbols_str.ToCharArray(0, symbols_str.Length);
			
			string[] favorable_list = (Console.In.ReadLine()).Split(' ');  
			dfa.IsFavorable = new bool[dfa.StatesCount + 1];
			foreach(string id in favorable_list)
				dfa.IsFavorable[Convert.ToInt32(id)] = true;
			
			dfa.StartState = Convert.ToInt32(Console.In.ReadLine()); 
			dfa.FavorableState = Convert.ToInt32(favorable_list[0]);
			
			dfa.Rules = new Dictionary<Leftside, int>();  
			string s;
			while((s = Console.In.ReadLine()) != "")   
			{
				string[] tr = s.Split(' ');
				dfa.Rules.Add(new Leftside(Convert.ToInt32(tr[0]), Convert.ToChar(tr[1])), Convert.ToInt32(tr[2]));
			}
		
			return dfa;
		}
		
		static Dictionary<Pair, bool> setupMarkedPairs(DFA dfa)
		{
			Dictionary<Pair, bool> pairs = new Dictionary<Pair, bool>();

			for(int i = 1; i <= dfa.StatesCount; ++i)
				for(int j = 1; j <= dfa.StatesCount; ++j)  
					{
						bool Marked = ((dfa.IsFavorable[i] == true && dfa.IsFavorable[j] == false) ||
									   (dfa.IsFavorable[i] == false && dfa.IsFavorable[j] == true));
						pairs.Add(new Pair(i, j), Marked);
					}
			
			return pairs;
		}
		
		static void processPairs(DFA dfa, Dictionary<Pair, bool> pairs)
		{
			bool found;
			do
			{
				found = false;
				foreach(KeyValuePair<Pair, bool> pair in pairs)
				{
					if(pair.Value == false)     
						foreach(char a in dfa.Symbols)
						{
							int d1 = dfa.Rules[new Leftside(pair.Key.P, a)];
							int d2 = dfa.Rules[new Leftside(pair.Key.Q, a)];

							if(pairs[new Pair(d1, d2)] == true) 
							{
								pairs[new Pair(pair.Key.P, pair.Key.Q)] = true;
								found = true;
								goto exit;
							}
						}
				}

				exit: ;
			}
			while(found);
		}
		
		static int[] createEqClasses(DFA dfa, Dictionary<Pair, bool> pairs)
		{
			int[] e_class = new int[dfa.StatesCount + 1];
			for(int i = 1; i <= dfa.StatesCount; ++i)
				e_class[i] = i;

			foreach(KeyValuePair<Pair, bool> pair in pairs)
				if(pair.Value == false)
					for(int i = 1; i <= dfa.StatesCount; ++i)
						if(e_class[i] == pair.Key.P)
							e_class[i] = pair.Key.Q;
			return e_class;
		}
		
		static void outputResults(DFA dfa, Dictionary<Pair, bool> pairs, int[] e_class)
		{
			Dictionary<int, bool> states = new Dictionary<int, bool>();
			for(int state = 1; state <= dfa.StatesCount; ++state)
				states[e_class[state]] = true;
			
			Dictionary<string, bool> rules = new Dictionary<string, bool>();
			foreach(KeyValuePair<Leftside, int> rule in dfa.Rules)
			{
				string rule_str = "(" + e_class[rule.Key.State].ToString() + ", " +
										rule.Key.Symbol.ToString() + ") -> " + e_class[rule.Value].ToString();
				
				rules[rule_str] = true;
			}
			
			Console.Write("States: "); 
			foreach(KeyValuePair<int, bool> state in states)
				Console.Write("{0} ", state.Key);
			
			Console.WriteLine();
			Console.WriteLine("Initial state: {0}\nFavorable state: {1}", 
								e_class[dfa.StartState], e_class[dfa.FavorableState]);
				
			Console.WriteLine("Rules:");                
			foreach(KeyValuePair<string, bool> rule in rules)
				Console.WriteLine(rule.Key);
		}
		
		static void Main(string[] args)
		{
			DFA dfa = readInput();

			Dictionary<Pair, bool> pairs = setupMarkedPairs(dfa); 
			processPairs(dfa, pairs);

			int[] e_class = createEqClasses(dfa, pairs);
			
			outputResults(dfa, pairs, e_class);
		}
	}
}
