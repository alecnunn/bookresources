<?php
  session_start();
?>

<HTML>
<HEAD>
<TITLE> Test page </TITLE>
</HEAD>

<BODY>

<?php
  if (!isset($_SESSION["user"]))
   {
    die("Autorization required");    
   }
  $t = $_SESSION["user"];
  print("<P>Hello: $t");
?>

<hr>
<center>
<p>Hackish PHP
<br>&copy; Michael Flenov 2005
</center><P>
</BODY>
<HTML>

