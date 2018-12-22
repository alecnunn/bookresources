<?php
 @session_start();
 session_register("username");
 session_register("password");
?>
<form action="authorize.php" method="post">
 <B>Authorization</B>
 <BR>User name: <input name="user">
 <BR>Password: <input type="password" name="pass">
 <P><input type="submit" value="Enter">
</form>

<?php
if (isset($user))
{
 $username=$user;
 $password=$pass;
}

if (($username=="admin") and ($password=="qwerty"))
{
 print("<HR>Hallo  $name<HR>");
 print("<P><A HREF=\"authorize.php?id=1\">Create object</A>"); 
 print("<P><A HREF=\"authorize.php?id=2\">Delete object</A>"); 
 
 if ($id==1)
 {
  // Create object code
  print("<P><I>Object created</I>");
 }

 if ($id==2)
 {
  // Delete object code
  print("<P><I>Object deleted</I>");
 }
}

?>