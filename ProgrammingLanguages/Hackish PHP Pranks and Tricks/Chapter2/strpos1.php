<HTML>
<HEAD>
<TITLE> Test page </TITLE>
</HEAD>

<BODY>
<p> 

<?php
 $start = -1;
 $text = "Hackish PHP Pranks&Tricks";
 while ($start<>0) 
  {
   $end = strpos($text, " ", $start+1);
   if ($end==0)
    $word = substr($text, $start+1, strlen($text)-$start-1);
   else    
    $word = substr($text, $start+1, $end-$start-1);
   print("Word: $word; <BR>");
   $start = $end;
  } 

 $end = strpos("Hackish PHP Pranks&Tricks", " ", $start+1);
 print("Second space: $index");
?>

<hr>
<center>
<p>Hackish PHP
<br>&copy; Michael Flenov 2005
</center><P>
</BODY>
<HTML>

