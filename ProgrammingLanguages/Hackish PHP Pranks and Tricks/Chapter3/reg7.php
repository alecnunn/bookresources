<HTML>
<HEAD>
<TITLE> Test page </TITLE>
</HEAD>

<BODY>

<?php
 $date= "1/9/2005";
 $newtext = ereg("([1]?[0-9])/([1-3]?[1-9])/(20[0-9][0-9])", $date, $regs);
 print("<P> Param 0 = $regs[0]");
 print("<P>Param 1 = $regs[1]");
 print("<P>Param 2 = $regs[2]");
 print("<P>Param 3 = $regs[3]");
?>

<hr>
<center>
<p>Hackish PHP
<br>&copy; Michael Flenov 2005
</center><P>
</BODY>
<HTML>

