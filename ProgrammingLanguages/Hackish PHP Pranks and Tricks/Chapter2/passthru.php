<HTML>
<HEAD>
<TITLE> Test page </TITLE>
</HEAD>

<BODY>

<?php
 if($f=fopen("/var/www/html/1/testfile.txt", "r"))
  {
   print("File opened ($f)");
  }
 else 
  {
   die("File open error");
  }
 
 fpassthru($f); 
?>

<hr>
<center>
<p>Hackish PHP
<br>&copy; Michael Flenov 2005
</center><P>
</BODY>
<HTML>

