<?php
 $host_ip=gethostbyname("localhost");
 $s=socket_create(AF_INET, SOCK_STREAM, 0);

 if (!($res=@socket_connect($s, $host_ip, 21)))
  die("Can' connect to local host");

 print("<P>Connected"); 

 printf("<P><%s", socket_read($s, 1000, PHP_NORMAL_READ)); 
 printf("<P><%s", socket_read($s, 1000, PHP_NORMAL_READ)); 

 $str="USER flenov\n";
 socket_write($s, $str, strlen($str));
 print("<P> > $str");
 printf("<P><%s", socket_read($s, 1000, PHP_NORMAL_READ)); 
 printf("<P>< %s", socket_read($s, 1000, PHP_NORMAL_READ)); 

 $str="PASS vampir\n";
 socket_write($s, $str, strlen($str));
 print("<P> > $str");
 printf("<P>< %s", socket_read($s, 1000, PHP_NORMAL_READ)); 
 printf("<P>< %s", socket_read($s, 1000, PHP_NORMAL_READ)); 

 $str="SYST\n";
 socket_write($s, $str, strlen($str));
 print("<P> > $str");
 printf("<P>< %s", socket_read($s, 1000, PHP_NORMAL_READ)); 
 printf("<P>< %s", socket_read($s, 1000, PHP_NORMAL_READ)); 
?>