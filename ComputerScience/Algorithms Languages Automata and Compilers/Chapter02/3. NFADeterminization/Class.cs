using System;
using System.Collections.Generic;

namespace NFADeterminization
{
	class Class
	{

		struct Pair             
		{
			public int P, Q;
			public Pair(int thep, int theq) { P = thep; Q = theq; }
		}

		struct Leftside         
		{
			public int State;
			public char Symbol;
			public Leftside(int st, char sym) { State = st; Symbol = sym; }

		}

		struct NFA
		{
			public int StatesCount;
			public char[] Symbols;
			public bool[] IsFavorable;
			public int StartState;
			public Dictionary<Leftside, List<int> > Rules;
		}

		static void generateNewStates(List<string> newStates, int depth, string name)
		{ 
			if(depth == 0)
				newStates.Add("{" + name);
			else
			{
				generateNewStates(newStates, depth - 1, " " + depth.ToString() + name);
				generateNewStates(newStates, depth - 1, name);
			}
		}
		
		static List<int> getStateElements(string state)
		{
			List<int> result = new List<int>();
			
			string[] state_elems = state.Split(' ');
			for(int i = 1; i < state_elems.Length - 1; i++)
				result.Add(Convert.ToInt32(state_elems[i]));

			return result;
		}
					
		static NFA readInput()
		{
			NFA nfa;
			nfa.StatesCount = Convert.ToInt32(Console.In.ReadLine());

			string symbols_str = Console.In.ReadLine();
			nfa.Symbols = symbols_str.ToCharArray(0, symbols_str.Length);
			
			string[] favorable_list = (Console.In.ReadLine()).Split(' ');
			nfa.IsFavorable = new bool[nfa.StatesCount + 1];
			foreach(string id in favorable_list)
				nfa.IsFavorable[Convert.ToInt32(id)] = true;
			
			nfa.StartState = Convert.ToInt32(Console.In.ReadLine()); 
			
			nfa.Rules = new Dictionary<Leftside, List<int> >(); 
			string s;
			while((s = Console.In.ReadLine()) != "")  
			{
				string[] tr = s.Split(' ');
				
				Leftside ls = new Leftside(Convert.ToInt32(tr[0]), Convert.ToChar(tr[1]));
				
				if(!nfa.Rules.ContainsKey(ls))
					nfa.Rules.Add(ls, new List<int>());
				
				nfa.Rules[ls].Add(Convert.ToInt32(tr[2]));
			}
		
			return nfa;
		}
		
		static Dictionary<string, List<int> > getNewRules(NFA nfa, List<string> newStates)
		{
			Dictionary<string, List<int> > newRules = new Dictionary<string, List<int> >();
			
			foreach(string state in newStates) 
				foreach(char c in nfa.Symbols) 
				{
					Dictionary<int, bool> R = new Dictionary<int, bool>();

					foreach(int Si in getStateElements(state))
					{
						if(nfa.Rules.ContainsKey(new Leftside(Si, c)))
						{	
							foreach(int r_state in nfa.Rules[new Leftside(Si, c)])
								R[r_state] = true;
						}
					}
					
					if(R.Count > 0)
					{
						List<int> r_states = new List<int>();
						
						foreach(KeyValuePair<int, bool> r_state in R)
							r_states.Add(r_state.Key);
						
						newRules[state + ", " + c] = r_states;
					}	
				}
			
			return newRules;
		}
		
		static void outputResults(NFA nfa, List<string> newStates, Dictionary<string, List<int> > newRules)
		{
			Console.WriteLine("Initial state: {{ {0} }}", nfa.StartState);
			Console.WriteLine("Favorable states:");

			foreach(string state in newStates)
			{
				foreach(int Si in getStateElements(state))
					if(nfa.IsFavorable[Si])
					{
						Console.Write(state + " ");
						break;
					}
			}
				
			Console.WriteLine("\nRules:");
			
			foreach(KeyValuePair<string, List<int> > rule in newRules)
			{
				Console.Write(rule.Key + " -> { ");
				foreach(int state in rule.Value)
					Console.Write(state + " ");
				Console.WriteLine("}");
			}
		}
		
		static void Main(string[] args)
		{
			NFA nfa = readInput();
			List<string> newStates = new List<string>();
			
			generateNewStates(newStates, nfa.StatesCount, " }");
			newStates.RemoveAt(newStates.Count - 1);
			
			Dictionary<string, List<int> > newRules = getNewRules(nfa, newStates);

			outputResults(nfa, newStates, newRules);
		}
	}
}

