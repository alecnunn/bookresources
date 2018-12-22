<HTML>
<HEAD>
<TITLE> Test page </TITLE>
</HEAD>

<BODY>

<?php
 $text= "BI IB IBB Hello world from PHP";
 $newtext = eregi_replace("BI|IB|IBB", "replaced", $text);
 echo($newtext);
?>

<hr>
<center>
<p>Hackish PHP
<br>&copy; Michael Flenov 2005
</center><P>
</BODY>
<HTML>

