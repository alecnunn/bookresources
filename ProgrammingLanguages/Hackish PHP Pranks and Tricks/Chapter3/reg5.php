<HTML>
<HEAD>
<TITLE> Test page </TITLE>
</HEAD>

<BODY>

<?php
 $text= "2511111111";
 $newtext = ereg_replace("51{4,}", "XX", $text);
 echo($newtext);
?>

<hr>
<center>
<p>Hackish PHP
<br>&copy; Michael Flenov 2005
</center><P>
</BODY>
<HTML>

