<?
 @session_start();
 session_register("username");
 session_register("password");
 session_register("secure");

 if (isset($susername) and (!isset($clearsecure))
 {
  $username=$susername;
  $password=$spassword;
 }
else
 {
  $secure="";
  $password = md5($password);
 }

 if (!isset($username))
  die("You must enter the system!");

 $username = check_param($username);
 $secure = check_param($secure);

// If the security variable is set, check it.
 if ($secure!="")
 {
  // Checking the $secure variable against the value in the database
   $query = DBQuery("SELECT * FROM UsersTable WHERE 
(user_name = '$username');
   $users = mysql_num_rows($query);
   if (!$users)
    die("Authentication failed");
   $user_data = mysql_fetch_array($query);
   if (($password == $user_data[password_field])  
       ($secure == $user_data[secure_field]))
     {
	// A valid user; access allowed
      $secure = md5(rand(1, 1000000));
      DBQuery("UPDATE UsersTable SET secure_field = '$secure' WHERE 
user_name = '$username'");

      setcookie("ssecure", $secure, mktime(0,0,0,1,1,2010));     }
   else
     {
	// Authentication failed; zeroing out the parameters
       setcookie("spassword", "", 0);
       setcookie("ssecure", "", 0);
     }
 }

// If the security variable is not set, then check
 // the name and password and set the security variable
 if (($username) and ($secure==""))
 {
   $query = DBQuery("SELECT * FROM UsersTable WHERE 
(user_name = '$username');

   $users = mysql_num_rows($query);
   if (!$users)
     die("Authentication failed");

   $user_data = mysql_fetch_array($query);
   if ($password = $userd[password_field]))
    {
     $secure = md5(rand(1, 1000000));
     DBQuery("UPDATE UsersTable SET secure_field = '$secure' 
WHERE user_name = '$username'");

     setcookie("susername", $username, mktime(0,0,0,1,1,2010));
     setcookie("spassword", $password, mktime(0,0,0,1,1,2010));
     setcookie("ssecure", $secure, mktime(0,0,0,1,1,2010));
     print("Welcome $ldata[0]");
    }
   else
    {
      print("Authentication failure");
    }
 }
?>

// The form to enter the user name and password 
 <form action="index.php" method="post">
  <B> System Login </B>
  Name: <input name="username" size="20">
  Password: <input type="password" name="password" size="20"><br />
  <input type="hidden" name="clearsecure" value="1">
  <input type="submit" value="Enter">
 </form>

function check_param($var)
{
  $var=preg_replace("/[^a-z0-9\., _\n]/i", "", $var);
  return $var;
}
