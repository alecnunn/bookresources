<form action="db2.php" method="get">
 <input name="name">
</form>

<?php 
 $name=preg_replace("/[^a-zA-Z0-9 ]|insert|delete|update/i", "", $name);
 print('SELECT * FROM Users WHERE name='.$name);
?>
