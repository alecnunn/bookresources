<HTML>
<HEAD>
<TITLE> Test page </TITLE>
</HEAD>

<BODY>


<form action="submit1.php" method="get">
User Name: <input name="UserName">
 <input type="hidden" name="Password" value="qwerty">
 <input type="submit" name="sub" value="Go">
</form>

<?php
  print("$UserName");
?>

<hr>
<center>
<p>Hackish PHP
<br>&copy; Michael Flenov 2005
</center><P>
</BODY>
<HTML>

