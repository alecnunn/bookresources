<?php 
 $s="/var/www/html/files/".$HTTP_POST_FILES["userfile"]["size"];
 print("$s");
 
 if (copy($PHP_POST_FILES["userfile"]["tmp_name"], 
   "/var/www/html/files/".$HTTP_POST_FILES["userfile"]["name"]))
  print("Complete");
 else 
  print("Error copy file 1");
?>
