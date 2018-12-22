<HTML>
<HEAD>
<TITLE> Test page </TITLE>
</HEAD>

<BODY>
<p> 

<?php

error_reporting(E_ALL - (E_NOTICE + E_WARNING));

function print_max($number1, $number2)
{
 print ("$number1 > $number2 = ");
 
 if ($number1>$number2)
  print ("true <BR>");
 else 
  print ("false <BR>");
}

print_max(10, 435);
print_max(3240, 2335);
print_max(sdf23, 45);
print_max(45);
?>

<hr>
<center>
<p>Hackish PHP
<br>&copy; Michael Flenov 2005
</center><P>
</BODY>
<HTML>

