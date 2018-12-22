<form action="env.php" method="get">
 <B>Type server name or IP address</B>
 <BR>Server: <input name="server">
 <BR><input type="submit" value="Ping">
</form>

<?php
 print($HTTP_REFERER);
?>