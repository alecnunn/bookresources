using System;
using System.IO;
using System.Text.RegularExpressions;

namespace EMailChange
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

			Regex r = new Regex("\\b(?<user>[a-zA-Z0-9_.-]+)@" +
								"(?<server>[a-zA-Z0-9._-]+)" +
			                    "\\.(?<domain>[a-zA-Z]{2,4})\\b");
			string result = r.Replace(text, "${user} at ${server} dot ${domain}");

			StreamWriter sw = new StreamWriter("new " + args[0], false, System.Text.Encoding.Default);
			sw.Write(result);
			sw.Close();
		}
	}
}
