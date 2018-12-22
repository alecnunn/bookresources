<?php
 $host_ip = gethostbyname("www.yahoo.com");
 for ($i=1; $i<=100; $i++)
 {
  $s=socket_create(AF_INET, SOCK_STREAM, 0);
  $res=@socket_connect($s, $host_ip, $i);
  if ($res)
   {
    $portname=getservbyport($i, "tcp");
    print("<P> Port opened $i ($portname)"); 
   }
 }
?>
