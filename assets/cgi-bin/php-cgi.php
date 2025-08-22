#!/usr/bin/env php
<?php
    $cmd="curl -X POST 127.0.0.1:8080/index.php";
    $out=shell_exec($cmd);


    echo  $out ;
    
?>