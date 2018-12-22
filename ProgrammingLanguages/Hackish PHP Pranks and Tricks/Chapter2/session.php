<?php
  if (@session_start())
   {
    print("OK");
   }
  $user=$UserName;
  session_register("user");
?>

<HTML>
<HEAD>
<TITLE> Test page </TITLE>
</HEAD>

<BODY>


<form action="session.php" method="get">
 User Name: <input name="UserName">
 <input type="submit" name="sub" value="Go">
</form>

<a href="session1.php">This is are link</a>

<hr>
<center>
<p>Hackish PHP
<br>&copy; Michael Flenov 2005
</center><P>
</BODY>
<HTML>

