using System;
using System.IO;
using System.Configuration;


	/// <summary>
	/// Summary description for MediaUtilities.
	/// </summary>
public class MediaUtilities {
	//		private static string defaultPath = @"..\..\media\";
	private static string mediaPath = ConfigurationSettings.AppSettings.Get("MediaPath");
		
	public MediaUtilities() {
	}

	public static string FindFile(string filename) {
		return FindFile(mediaPath, filename);
	}

	public static string FindFile(string path, string filename) {
		// First try to load the file in the full path
		string fullName = AppendDirectorySeparator(path) + filename;
		if (File.Exists(fullName))
			return fullName;
		else
			throw new FileNotFoundException("Could not find this file.", filename);
	}

	private static string AppendDirectorySeparator(string pathname) {
		if (!pathname.EndsWith(@"\"))
			return pathname + @"\";
		else
			return pathname;
	}
}
