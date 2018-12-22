<?php
 $i=1;
 for ($i=1; $i<=100; $i++)
 {
  $s=socket_create(AF_INET, SOCK_STREAM, 0);
  $res=@socket_connect($s, "127.0.0.1", $i);
  if ($res)
   {
    print("<P>Port opened $i");    
   }
 }
?>