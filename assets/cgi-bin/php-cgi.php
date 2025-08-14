#!/usr/bin/env php
<?php

// You must send headers manually
echo "Content-Type: text/html\r\n\r\n";

function html_escape($text) {
    return htmlspecialchars($text, ENT_QUOTES | ENT_HTML5, 'UTF-8');
}

$method = getenv('REQUEST_METHOD') ?: 'GET';

echo "<!DOCTYPE html>";
echo "<html><head><title>CGI Response</title></head><body>";
echo "<h1>Hello from PHP CLI acting like CGI!</h1>";
echo "<p>Request Method: <strong>" . html_escape($method) . "</strong></p>";

if ($method === 'GET') {
    $query = getenv('QUERY_STRING') ?: '';
    parse_str($query, $data);
    echo "<p>Query String: <code>" . html_escape($query) . "</code></p>";
} elseif ($method === 'POST') {
    $data = [];
    $raw = file_get_contents("php://stdin");
    parse_str($raw, $data);
    echo "<p>Raw POST Data: <code>" . html_escape($raw) . "</code></p>";
} else {
    $data = [];
}

if (!empty($data)) {
    echo "<h2>Parsed Form Data:</h2><ul>";
    foreach ($data as $key => $value) {
        echo "<li>" . html_escape($key) . " = " . html_escape($value) . "</li>";
    }
    echo "</ul>";
}

echo "</body></html>";