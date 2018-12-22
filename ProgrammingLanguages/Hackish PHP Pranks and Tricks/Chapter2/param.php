<HTML>
<HEAD>
<TITLE> Test page </TITLE>
</HEAD>

<BODY>
<p> 

<P>Authorize
<form action="param.php" method="get">
User Name: <input name="UserName">
 <input type="hidden" name="Password" value="qwerty">
</form>

<?php
 if ($UserName<>"")
  {
   print("<P>Your user name: ");
   print("$UserName");
   print("<P>Your Password: $Password");
  } 
?>

<hr>
<center>
<p>Hackish PHP
<br>&copy; Michael Flenov 2005
</center><P>
</BODY>
<HTML>

