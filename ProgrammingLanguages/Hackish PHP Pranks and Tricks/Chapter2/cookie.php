<?php
 $access++;
 setcookie("access", $access, mktime(0,0,0,1,1,2010));
?>

<HTML>
<HEAD>
<TITLE> Test page </TITLE>
</HEAD>

<BODY>

<?php
 print("You've seen this page $access times");
?>

<hr>
<center>
<p>Hackish PHP
<br>&copy; Michael Flenov 2005
</center><P>
</BODY>
<HTML>

