<?php
  if (@session_start())
   {
    print("OK");
   }
  $_SESSION["user"] = $UserName;
?>
<HTML>
<HEAD>
<TITLE> Test page </TITLE>
</HEAD>

<BODY>


<form action="session2.php" method="get">
 User Name: <input name="UserName">
 <input type="submit" name="sub" value="Go">
</form>

<a href="session3.php">This is are link</a>


<hr>
<center>
<p>Hackish PHP
<br>&copy; Michael Flenov 2005
</center><P>
</BODY>
<HTML>

