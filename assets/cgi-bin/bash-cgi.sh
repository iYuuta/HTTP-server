#!/bin/bash

# Required CGI header with explicit CRLF
printf "Content-Type: text/html\r\n"
printf "\r\n"

# HTML escape function
html_escape() {
    echo "$1" | sed -e 's/&/\&amp;/g' \
                   -e 's/</\&lt;/g' \
                   -e 's/>/\&gt;/g' \
                   -e 's/\"/\&quot;/g' \
                   -e "s/'/\&#x27;/g"
}

method=${REQUEST_METHOD:-GET}
query=""
if [ "$method" = "GET" ]; then
    query="$QUERY_STRING"
elif [ "$method" = "POST" ]; then
    read -n "$CONTENT_LENGTH" query
fi

echo "<!DOCTYPE html>"
echo "<html><head><title>CGI Response</title></head><body>"
echo "<h1>Hello from Bash CGI!</h1>"
echo "<p>Request Method: <strong>$method</strong></p>"
echo "<p>Raw Data: <code>$(html_escape "$query")</code></p>"

# Parse key-value pairs
if [ -n "$query" ]; then
    echo "<h2>Parsed Form Data:</h2><ul>"
    IFS='&' read -ra pairs <<< "$query"
    for pair in "${pairs[@]}"; do
        IFS='=' read -r key value <<< "$pair"
        key=$(html_escape "$(printf "%b" "${key//%/\\x}")")
        value=$(html_escape "$(printf "%b" "${value//%/\\x}")")
        echo "<li>$key = $value</li>"
    done
    echo "</ul>"
fi

echo "</body></html>"
