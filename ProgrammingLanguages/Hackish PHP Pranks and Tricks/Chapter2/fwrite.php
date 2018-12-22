<HTML>
<HEAD>
<TITLE> Test page </TITLE>
</HEAD>

<BODY>

<?php
 if(!($f=fopen("/var/www/html/1/testfile.txt", "r+")))
  {
   die("File open error");
  }

 $s = fread($f, 7);
 print("<P>Line 1: $s"); 
 
 fwrite($f, "writing");

 fclose($f); 
?>

<hr>
<center>
<p>Hackish PHP
<br>&copy; Michael Flenov 2005
</center><P>
</BODY>
<HTML>

