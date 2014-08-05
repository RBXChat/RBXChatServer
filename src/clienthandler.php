<?php
	error_reporting(E_ALL);
	session_id("temp");
	echo session_start();
	echo session_name();
	$sock = $_SESSION['sock'];
	var_dump($_SESSION);
	$name = "";
	socket_getpeername($sock, $name);
	$sockets[] = $name;
	var_dump($name);
	//ob_end_flush(); 
    //ob_flush(); 
    //flush(); 
    //ob_start(); 
	while(1);
?>