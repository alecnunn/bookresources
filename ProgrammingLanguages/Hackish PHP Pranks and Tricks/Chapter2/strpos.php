<HTML>
<HEAD>
<TITLE> Test page </TITLE>
</HEAD>

<BODY>
<p> 

<?php
 $index = strpos("Hackish PHP Pranks&Tricks", "PHP");
 print("PHP Place: $index <BR>");

 $index = strpos("Hackish PHP Pranks&Tricks", " ");
 print("First space: $index <BR>");

 $index = strpos("Hackish PHP Pranks&Tricks", " ", $index+1);
 print("Second space: $index");
?>

<hr>
<center>
<p>Hackish PHP
<br>&copy; Michael Flenov 2005
</center><P>
</BODY>
<HTML>

