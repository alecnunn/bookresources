<?php
 function ReadCache($CacheName)
 {
    if (file_exists("cache/$CacheName.htm"))
    {
       require("cache/$CacheName.htm");
       print("<HR>Loaded from cache");
       return 1;
    }   
    return 0;
 }

 function WriteCache($CacheName, $CacheData)
 {
    $cf = @fopen("cache/$CacheName.htm", "w") 
      or die ("Can't write cache");
    fputs ($cf, $CacheData);
    fclose ($cf);
    @chmod ("cache/$CacheName.htm", 0777);
 }
 if (ReadCache("MainPage")==1)
  exit;
 
 ob_start();
 print("<CENTER><B>Main page</B></CENTER>");
 print("<P>This is test");
 WriteCache("MainPage", ob_get_contents());
 ob_end_flush();
?>
