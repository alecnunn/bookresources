<?php 
 print("<P>Temp file name $file1");
 print("<P>File name $file1_name");
 print("<P>File size $file1_size");
 print("<P>File type $file1_type");
 
 if (copy($file1, 
   "/var/www/html/files/".$file1_name))
  print("<P><B>Complete</B>");
 else 
  print("5Error copy file 1</B>");
?>
