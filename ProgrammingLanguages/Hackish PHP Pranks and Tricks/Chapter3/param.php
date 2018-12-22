<?php
 $str = "<I><STRONG>Hallo <B> World<SCRIPT>";
 $str=ereg_replace("<[A-Z]{1,}[^BI]>", "", $str);
 print($str);
?>