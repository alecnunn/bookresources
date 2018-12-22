<HTML>
<HEAD>
<TITLE> Test page </TITLE>
</HEAD>

<BODY>
<p> 

<P>Authorize
<form action="arrayparam.php" method="post">
User Name: <input name="UserName">
 <input type="hidden" name="Password" value="qwerty">
</form>

<?php
 if (count($HTTP_GET_VARS)>0)
  {
   die("Wrong parameters");
  }
 
 if ($HTTP_POST_VARS["UserName"]<>"")
  {
   print("<P>Your user name: ");
   print($HTTP_POST_VARS["UserName"]);
   print("<P>Your Password: ");   
   print($HTTP_POST_VARS["Password"]);
  } 
?>

<hr>
<center>
<p>Hackish PHP
<br>&copy; Michael Flenov 2005
</center><P>
</BODY>
<HTML>

