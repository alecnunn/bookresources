<HTML>
<HEAD>
<TITLE> Test page </TITLE>
</HEAD>

<BODY>

<?php
 if($f=fopen("testfile.txt", "w+"))
  {
   print("File opened ($f)");
  }
 else 
  {
   print("File open error");
  }
?>

<hr>
<center>
<p>Hackish PHP
<br>&copy; Michael Flenov 2005
</center><P>
</BODY>
<HTML>

