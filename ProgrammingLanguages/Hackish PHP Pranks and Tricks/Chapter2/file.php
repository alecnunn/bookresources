<HTML>
<HEAD>
<TITLE> Test page </TITLE>
</HEAD>

<BODY>

<?php
 if ($arr=file("/var/www/html/1/testfile.txt"))
  {
   for ($i=0; $i<count($arr); $i++)
     { 
       printf("<BR>%s", $arr[$i]);
     }
  }
?>

<hr>
<center>
<p>Hackish PHP
<br>&copy; Michael Flenov 2005
</center><P>
</BODY>
<HTML>

