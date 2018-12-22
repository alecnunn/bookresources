using System;
using System.Collections.Generic;

namespace EMailDFA
{
	/// <summary>
	/// Summary description for Class.
	/// </summary>
	class Class
	{
		/// <summary>
		/// The main entry point for the application.
		/// </summary>

		enum StateType { s1, s2, s3, s4, s5, sF };
        enum CommandType { cMain, cDot, cAt, cEnd };

		struct SC
        {
            public StateType state;
            public CommandType command;
            public SC(StateType state_, CommandType command_)
            {
                state = state_;
                command = command_;
            }
        }

		static CommandType convertCommand(char c)
		{
			if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
			   (c >= '0' && c <= '9') || (c == '_' || c == '-'))
				return CommandType.cMain;
			if(c == '@') return CommandType.cAt;
			if(c == '.') return CommandType.cDot;
			
			return CommandType.cEnd;
		}
		
		static void Main(string[] args)
		{
			string Tape = Console.ReadLine() + '\0';
			StateType State = StateType.s1;
			CommandType Command;
			
			Dictionary<SC, StateType> Automaton = new Dictionary<SC, StateType>();
			Automaton.Add(new SC(StateType.s1, CommandType.cMain), StateType.s2);
			Automaton.Add(new SC(StateType.s2, CommandType.cMain), StateType.s2);
			Automaton.Add(new SC(StateType.s2, CommandType.cAt), StateType.s3);
			Automaton.Add(new SC(StateType.s3, CommandType.cMain), StateType.s4);
			Automaton.Add(new SC(StateType.s4, CommandType.cMain), StateType.s4);
			Automaton.Add(new SC(StateType.s4, CommandType.cDot), StateType.s5);
			Automaton.Add(new SC(StateType.s5, CommandType.cMain), StateType.sF);
			Automaton.Add(new SC(StateType.sF, CommandType.cMain), StateType.sF);
			Automaton.Add(new SC(StateType.sF, CommandType.cDot), StateType.s5);
			
			
			try
            {
                while(true)
                {
                    Console.WriteLine("State: " + State.ToString());

                    Command = convertCommand(Tape[0]);
					Tape = Tape.Substring(1);
					
                    if(Command == CommandType.cEnd) 
                        break;  

                    State = Automaton[new SC(State, Command)];
                }
            }
            catch(Exception)
            {
                Console.WriteLine("Incorrect transition"); 
            }
		}
	}
}
