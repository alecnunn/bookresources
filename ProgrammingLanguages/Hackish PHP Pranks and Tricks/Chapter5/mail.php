<?php
$MailTo = "recipient@mail_server.com";
$MailSubj = "This is subject";

$MailFrom = "your_name@your_server.com";
$MailCC = "name1@@mail_server.com,name2@@mail_server.com";
$Extra = "From: $MailFrom\r\nCc: $MailCC";

$MailBody = "This is mail body";

if(mail($MailTo, $MailSubj, $MailBody, $Extra))
 print('Success');
else 
 print('Error');
?>
