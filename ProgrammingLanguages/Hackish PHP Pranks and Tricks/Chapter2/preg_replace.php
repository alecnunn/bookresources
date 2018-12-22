<HTML>
<HEAD>
<TITLE> Test page </TITLE>
</HEAD>

<BODY>

<?php
 $text= "Hello world from PHP";
 $newtext = preg_replace("/world/i", "Sam", $text);
 echo ($newtext);
?>

<hr>
<center>
<p>Hackish PHP
<br>&copy; Michael Flenov 2005
</center><P>
</BODY>
<HTML>

