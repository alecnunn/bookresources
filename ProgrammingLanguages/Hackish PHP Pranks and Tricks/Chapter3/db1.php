<form action="db1.php" method="get">
 <input name="id">
</form>

<?php 
 $id=preg_replace("/[^0-9]/", "", $id);
 print('SELECT * FROM Users WHERE id='.$id);
?>
