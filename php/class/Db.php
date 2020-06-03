<?php
require __DIR__ . '/MysqliDb/MysqliDb.php';
$host='localhost:3306';
$username='ticketsystem';
$password='password';
$dbname='ticketsystem';
$db=new MysqliDb($host,$username,$password,$dbname);
