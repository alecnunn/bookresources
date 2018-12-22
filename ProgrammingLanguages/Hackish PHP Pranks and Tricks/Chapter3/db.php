<?php
 if (mysql_connect("localhost")==0)
  die("Can't connect to Database Server");

 mysql_select_db("vronline");

 $result=mysql_query("SELECT * FROM cyddata_st_stories");
 
 $rows=mysql_num_rows($result);
 print($rows);

 mysql_close();
?>