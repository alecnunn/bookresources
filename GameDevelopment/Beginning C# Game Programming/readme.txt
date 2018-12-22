"Beginning .NET Game Programming in C#", by David Weller, Alexandre Lobao, and Ellen Hatton.

***  README file:  20 March 2004 ***

First of all, thanks for buying our book!  If you downloaded this code without buying the book, we would like to ask (ok, beg) you to buy the book from your local bookstore of favorite online bookstore.  We made a deliberate decision to not include a CD-ROM in the book and make the code freely available for two reasons:

	1) Bugs and code improvements are inevitable, especially in books like this.  Our guess is that it would have only been a matter of weeks before somebody found a bug or suggested a better way of doing a certain part of code, so any CD-ROM included in the book would have the side effect of almost immediate obsolesence

	2) Adding a CD-ROM to a book raises the price of the book, and the benefits are negligible (see item #1).  


The code was written using Visual Studio .NET 2003 and the DirectX 9.0b SDK.  For those of you using other software development tools (like Borland's C#Builder or SharpDevelop), you will need to reorganize some of the code to correctly run.  This is particularly true in the earlier examples, where the locations of image files are hardcoded into the source.

There is one game that, unfortunately, is highly buggy -- Space Donuts.  Due to time contstraints, we wrote the code in a bit of a hurry, and the result is that you see some very odd behaviour.  Specifically:
	- Transition between full-screen and windowed mode doesn't work
	- Border-collision checks sometimes cause the sprites to "hug" the border
	- Unnecessary updates to all sprites causes a lot of inefficiency.
Rest assured that Space Donuts will have an update very shortly.  We're a little embarassed by this one :-(

If you have questions about the code, feel free to send a note to David Weller (david@inkblog.com).

Enjoy!


