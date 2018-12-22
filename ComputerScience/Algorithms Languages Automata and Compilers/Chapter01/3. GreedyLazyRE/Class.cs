using System;
using System.IO;
using System.Text.RegularExpressions;

namespace GreedyLazyRE
{
	/// <summary>
	/// Summary description for Class.
	/// </summary>
	class Class
	{
		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main(string[] args)
		{
			StreamReader sr = new StreamReader(args[0], System.Text.Encoding.Default);
			string text = sr.ReadToEnd();              
			sr.Close();                                

            Regex r1 = new Regex("1\\..+\\d\\d\\d-\\d\\d\\d\\d", RegexOptions.Singleline);
			Regex r2 = new Regex("\\[imp].*?\\[/imp]", RegexOptions.Singleline);

			Match m = r1.Match(text);
			Console.WriteLine(m.Value);
			Console.WriteLine();

			MatchCollection mc = r2.Matches(text);
			foreach(Match mt in mc)
				Console.WriteLine(mt.Value.Substring(5, mt.Value.Length - 11));
		}
	}
}
