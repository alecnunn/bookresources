<?php
 $str="This is test";
 print("<P>Encode string $str");
 $encoded=base64_encode($str);
 print("<P>Encoded: $encoded");
 $decoded=base64_decode($encoded);
 print("<P>Decoded: $decoded");
?>
