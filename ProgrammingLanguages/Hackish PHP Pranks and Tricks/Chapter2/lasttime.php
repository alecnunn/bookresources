<HTML>
<HEAD>
<TITLE> Test page </TITLE>
</HEAD>

<BODY>

<?php
 if ($time=filectime("testfile.txt"))
  {
   $timestr = date("l dS of F Y h:i:s A", $time);
   print("Last modified: $timestr");
  }
?>

<hr>
<center>
<p>Hackish PHP
<br>&copy; Michael Flenov 2005
</center><P>
</BODY>
<HTML>

