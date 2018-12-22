<HTML>
<HEAD>
<TITLE> Test page </TITLE>
</HEAD>

<BODY>

<?php
 $date= "01/09/2005";
 $newtext = ereg("([0-9]{1,2})/([0-9]{1,2})/([0-9]{2,4})", $date, $regs);
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

