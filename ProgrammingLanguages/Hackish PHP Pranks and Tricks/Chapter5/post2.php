<?php 
 if ($file1=="none")
  die("File too much");
  
 print("<P>Temp file name $file1");
 print("<P>File name $file1_name");
 print("<P>File size $file1_size");
 print("<P>File type $file1_type");
 
 preg_match("'([a-z]+)\/[x\-]*([a-z]+)'", $file1_type, $ext);
 print("<P>$ext[1]");
 print("<P>$ext[2]");

 switch($ext[2])
 {
   case "jpg":
   case "jpeg":
   case "pjpeg":
   case "gif":
   case "png":
    break;  
   default:
    die("<P>This is not image");
 }    

 if (copy($file1,"/var/www/html/files/".$file1_name))
  print("<P><B>Complete</B>");
 else 
  print("<P>Error copy file 1</B>");
?>
