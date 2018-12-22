<HTML>
<HEAD>
<TITLE> Test page </TITLE>
</HEAD>

<BODY>
<p> 

<?php

 $Sub_string = substr("Hackish PHP", 8, 3);
 print($Sub_string);
 print("<br>");	

 $Sub_string = substr("Hackish PHP", 8);
 print($Sub_string);
 print("<br>");	
 
 $Sub_string = substr("Hackish", 0, 4);
 print($Sub_string);
 
?>

<hr>
<center>
<p>Hackish PHP
<br>&copy; Michael Flenov 2005
</center><P>
</BODY>
<HTML>

