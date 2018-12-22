using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace TM
{
	class Class
	{
		struct LeftSide
		{
			public int State;
			public char Symbol;
			public LeftSide(int st, char sym) { State = st; Symbol = sym; }
		}
 
		struct RightSide
		{
			public int State;
			public char Symbol, Command;
			public RightSide(int st, char sym, char c) { State = st; Symbol = sym; Command = c; }
		}

		[DllImport("user32.dll", EntryPoint = "GetKeyState")]
		public static extern short GetKeyState(int nVirtKey);
		
		static void Main(string[] args)
		{
			int Sstate = Convert.ToInt32(Console.ReadLine());
			int Ystate = Convert.ToInt32(Console.ReadLine());
			int Nstate = Convert.ToInt32(Console.ReadLine());
			string Tape = ">" + Console.ReadLine() + "_";
			
			Dictionary<LeftSide, RightSide> Rules = new Dictionary<LeftSide, RightSide>();

			string s;
			while((s = Console.ReadLine()) != "")
			{
				string[] tr = s.Split(' ');  // state symbol state' symbol' command
				Rules.Add(new LeftSide(Convert.ToInt32(tr[0]), Convert.ToChar(tr[1])),
				          new RightSide(Convert.ToInt32(tr[2]), Convert.ToChar(tr[3]), Convert.ToChar(tr[4])));
			}
			
			int State = Sstate;
			int TapeIdx = 1;
			const int VK_SPACE = 0x20;
			try
			{
				while(State != Ystate && State != Nstate)
				{
			    	if((GetKeyState(VK_SPACE) & 0x8000) != 0)
					{ 
						Console.WriteLine("Work interrupted"); 
						return; 
					}
					
					RightSide r = Rules[new LeftSide(State, Tape[TapeIdx])];
					State = r.State;
					Tape = (Tape.Remove(TapeIdx, 1)).Insert(TapeIdx, r.Symbol.ToString());
						
					if(r.Command == 'L')
						TapeIdx--;
					else if(r.Command == 'R')
						TapeIdx++;
					
					if(TapeIdx >= Tape.Length)
						Tape += "_";
				}

				Console.WriteLine(State == Ystate ? "String accepted" : "String rejected");
			}
			catch(Exception)
            {
                Console.WriteLine("String rejected");
            }
			
			Console.WriteLine("Tape: " + Tape);
		}
	}
}
