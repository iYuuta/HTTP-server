#!/usr/bin/env php
<?php
    $cmd="curl 127.0.0.1:8090/cgi-bin/php-cgi.php";
    $out=shell_exec($cmd);
?>