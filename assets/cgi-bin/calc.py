#!/usr/bin/env python3
import sys
import urllib.parse

# Read POST body from stdin
body = sys.stdin.read()  # In real CGI, you may want to use CONTENT_LENGTH
data = urllib.parse.parse_qs(body)

# Extract parameters
try:
    num1 = int(data.get('num1', [0])[0])
    num2 = int(data.get('num2', [0])[0])
    oper = data.get('oper', [''])[0]
    oper = urllib.parse.unquote_plus(oper)  # decode %xx
    oper = oper.replace(' ', '+')           # convert '+' that became space


except ValueError:
    print("Content-Type: text/html\r\n")
    print("<html><body><h1>Invalid number</h1></body></html>")
    sys.exit(1)

# Compute result
if oper == '+':
    result = num1 + num2
elif oper == '-':
    result = num1 - num2
elif oper == '*':
    result = num1 * num2
elif oper == '/':
    if num2 == 0:
        result = "Division by zero"
    else:
        result = round(num1 / num2, 2)
else:
    result = "Unknown operator"

# Return valid HTTP response with HTML
print("Content-Type: text/html\r\n\r\n")
print(f"""<!DOCTYPE html>
<html>
<head>
    <title>CGI Calculation Result</title>
</head>
<body>
    <h1>Calculation Result</h1>
    <p>{num1} {oper} {num2} = <strong>{result}</strong></p>
</body>
</html>""")
