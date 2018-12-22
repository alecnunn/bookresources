<form action="ping.php" method="get">
 <B>Type server name or IP address</B>
 <BR>Server: <input name="server">
 <BR><input type="submit" value="Ping">
</form>

<?php
 if (!isset($server)) 
  exit;

 $server=preg_replace("/[^a-z0-9-_\.]/i", "", $server);

 $i=1;
 while ($i<3)
 {
   ereg("([0-9]{1,3})\.([0-9]{1,3})\.([0-9]{1,3})\.([0-9]{1,3})", 
     $server, $regs);

   print("<HR>Ping server $regs[1].$regs[2].$regs[3].$i");
   exec("ping -c 1 $regs[1].$regs[2].$regs[3].$i > ping.txt", $list);

   print("<PRE>");
   readfile("ping.txt");
   print("</PRE>");

   $i++;
 }

?>