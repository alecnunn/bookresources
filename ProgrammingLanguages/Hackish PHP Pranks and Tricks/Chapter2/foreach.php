<HTML>
<HEAD>
<TITLE> Test page </TITLE>
</HEAD>

<BODY>

<?php
 $goods[]= "cake";
 $goods[]= "bread";
 $goods[]= "carrot";
 
 foreach($goods as $Ind => $Val)
  {
   print("<P> index: $Ind <BR>value: $Val");
  }
?>

<hr>
<center>
<p>Hackish PHP
<br>&copy; Michael Flenov 2005
</center><P>
</BODY>
<HTML>

