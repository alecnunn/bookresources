using System;
using System.Collections.Generic;

namespace LiftAutomaton
{
	/// <summary>
	/// Summary description for Class.
	/// </summary>
	class Class
	{
		enum StateType { oD, cD, cU, oU };

		static void Main(string[] args)
		{
			string program = Console.ReadLine() + '\0';
			StateType State = StateType.oD;
			int i = 0;
			char symbol;

			try
			{
				while((symbol = program[i++]) != '\0')
				{
					Console.WriteLine("Current state: {0}", State);
					switch(State)
					{
						case StateType.oD: switch(symbol)
										   {
											   case 'c': State = StateType.cD; break;
											   default:  throw new Exception();
										   }
										   break;
						case StateType.cD: switch(symbol)
										   {
											   case 'o': State = StateType.oD; break;
											   case 'U': State = StateType.cU; break;
											   default:  throw new Exception();
										   }
										   break;
						case StateType.cU: switch(symbol)
										   {
											   case 'D': State = StateType.cD; break;
											   case 'o': State = StateType.oU; break;
											   default:  throw new Exception();
										   }
										   break;
						case StateType.oU: switch(symbol)
										   {
											   case 'c': State = StateType.cU; break;
											   default:  throw new Exception();
										   }
										   break;
					}
				}
				Console.WriteLine("Current state: {0}", State);
			}
			catch(Exception)
			{
				Console.WriteLine("Incorrect transition");
			}
		}
	}
}
