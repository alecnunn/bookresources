using System;
using System.Text.RegularExpressions;

namespace NumRegExp
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
			string s;
			Regex r = new Regex("\\A-?[0-9]+(.[0-9]+)?\\z");

			while((s = Console.ReadLine()) != "")
				Console.WriteLine(r.Matches(s).Count == 1);
		}
	}
}
