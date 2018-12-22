<?php
 $access[0]=$access[0]+1;
 $access[1]=$access[1]+2;
 setcookie("access[0]", $access[0], mktime(0,0,0,1,1,2010));
 setcookie("access[1]", $access[1], mktime(0,0,0,1,1,2010));
?>

<HTML>
<HEAD>
<TITLE> Test page </TITLE>
</HEAD>

<BODY>

<?php
 print("$access[0] : $access[1]");
?>

<hr>
<center>
<p>Hackish PHP
<br>&copy; Michael Flenov 2005
</center><P>
</BODY>
<HTML>

