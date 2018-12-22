<?
// Displaying the form to enter the registration data
if (($username=="") and (!isset($id)))
 {
?>
  <form action="register.php" method="post">
   // A form with fields to enter user information	
   User name <input name="username" size="32">
   Password <input name="pass1" type="password" size="32">
   Confirm the password <input name="pass2" type="password" size="32">
   E-mail <input name="email" size="32">
   <input type="submit" value="Register">
  </form>
<?
 }

// Registering a user
if ((username!="")
 {
  if ($pass1 != $pass2)
   die("The password was not correctly confirmed");

  // The code for connecting to the database goes in here

  // Deleting accounts not activated within 2 hours after creation
  $tmax=time()–7200;
  DBQuery("DELETE FROM users WHERE reg_time < $tmax AND active=0");

 // Checking for registered users with the same name or email 
  $query = DBQuery("SELECT * FROM users WHERE user_name = 
'$username' or email = '$email'");
  if (mysql_num_rows($query))
   die("User with this name already exists");
  
  $activatekey = md5(rand(1, 1000000)).$username;
  $password=md5($pass1);
  $userrtime = time();

  DBQuery("INSERT INTO users (user_name, pass, reg_time, email, key,
active) VALUES ('$username', '$password', '$userrtime', '$email', 
'$activatekey', '0')");  

 $mailbody = "Your account has been placed into the confirmation queue.\n\n To confirm the registration click on this link:\n http://www.yoursite.com/register.php?id=$activatekey \n\n Thank You";

 // Sending an email with the text of the $mailbody variable
 // to the address $email

 }

// Activating the user
if (isset($id))
 {
   $result = DBQuery("SELECT * FROM users WHERE key = '$id' and active=0");
   $data = mysql_fetch_array($result);
   if (!mysql_num_rows($result)) 
    die("No such code. Your registration has probably expired");

   DBQuery("UPDATE users SET active=1, key="" WHERE key = '$id' and active=0");
   print("Activation successful");
 }
?>
