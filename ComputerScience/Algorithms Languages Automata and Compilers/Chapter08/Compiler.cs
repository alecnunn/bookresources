using System;

public class Compiler 
{ 
    public static void Main(string[] arg) 
    { 
		try
		{
			Scanner scanner = new Scanner(arg[0]); 
        
			Parser parser = new Parser(scanner);
			parser.Parse();
            if(parser.errors.count == 0) 
                parser.RunProgram();
		}
		catch(Exception e)
		{
			Console.WriteLine("exception: " + e.Message);
		}
    } 
}
