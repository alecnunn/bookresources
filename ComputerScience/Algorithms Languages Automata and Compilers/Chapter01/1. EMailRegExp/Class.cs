using System;
using System.IO;
using System.Text.RegularExpressions;

namespace EMailRegExp
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

			Regex r = new Regex("\\b[a-zA-Z0-9._-]+@[a-zA-Z0-9._-]+\\.[a-zA-Z]{2,4}\\b");
			MatchCollection mc = r.Matches(text);

			foreach(Match m in mc)
				Console.WriteLine(m.Value);
		}
	}
}
