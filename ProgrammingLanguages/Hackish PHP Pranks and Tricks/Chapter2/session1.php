<?php
  session_start();
?>

<HTML>
<HEAD>
<TITLE> Test page </TITLE>
</HEAD>

<BODY>

<?php
  if (!isset($user))
   {
    die("Autorize require");    
   }
  print("<P>Hello: $user");
?>

<hr>
<center>
<p>Hackish PHP
<br>&copy; Michael Flenov 2005
</center><P>
</BODY>
<HTML>

