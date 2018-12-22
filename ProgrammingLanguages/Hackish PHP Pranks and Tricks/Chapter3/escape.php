<HTML>
<HEAD>
<TITLE> Test page </TITLE>
</HEAD>

<BODY>

<?php
 $text= "[B]Hello[/B] world from [B]PHP[/B]";
 $newtext = ereg_replace("\[B\]", "<B>", $text);
 $newtext = ereg_replace("\[/B\]", "</B>", $newtext);
 echo ($newtext);
?>

<hr>
<center>
<p>Hackish PHP
<br>&copy; Michael Flenov 2005
</center><P>
</BODY>
<HTML>

