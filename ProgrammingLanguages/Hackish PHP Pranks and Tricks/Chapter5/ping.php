<form action="ping.php" method="get">
 <B>Type server name or IP address</B>
 <BR>Server: <input name="server">
 <BR><input type="submit" value="Ping">
</form>

<?php
 if (!isset($server)) 
  exit;

 $server=preg_replace("/[^a-z0-9-_\.]/i", "", $server);

 print("<HR>Ping server $server");
 exec("ping -c 1 $server > ping.txt", $list);

 print("<PRE>");
 readfile("ping.txt");
 print("</PRE>");
?>