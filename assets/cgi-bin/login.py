#!/usr/bin/env python3

import base64
import fcntl
import hashlib
import json
import os
import sys
from urllib.parse import parse_qs


def send(content, set_cookie_value=None):
    body_bytes = content.encode("utf-8")
    print("Content-Type: text/html")
    if set_cookie_value is not None:
        print(f"Set-Cookie: token={set_cookie_value}; Path=/; HttpOnly; SameSite=Strict")
    print(f"Content-Length: {len(body_bytes)}")
    print()
    sys.stdout.flush()
    sys.stdout.buffer.write(body_bytes)


def get_login_page():
    return """<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8" />
<title>Secure Login</title>
<style>
  body {
    background: linear-gradient(135deg, #004d40 0%, #00695c 100%);
    font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
    margin: 0;
    height: 100vh;
    display: flex;
    justify-content: center;
    align-items: center;
    color: #e0f2f1;
  }
.login-container {
    background: rgba(38, 50, 56, 0.85);
    padding: 30px 40px;
    border-radius: 12px;
    box-shadow: 0 6px 25px rgba(0, 0, 0, 0.6);
    width: 480px;
    display: flex;
    flex-direction: column;
}

  form {
    display: flex;
    justify-content: space-between;
  }
  .form-group {
    display: flex;
    flex-direction: column;
    width: 48%;
  }
  label {
    font-weight: 600;
    margin-bottom: 8px;
    color: #80cbc4;
  }
  input[type="text"],
  input[type="password"] {
    padding: 10px 12px;
    border-radius: 6px;
    border: none;
    background: #004d40;
    color: #a7ffeb;
    font-size: 1em;
    outline-offset: 2px;
    outline-color: transparent;
    transition: outline-color 0.25s ease;
  }
  input[type="text"]:focus,
  input[type="password"]:focus {
    outline-color: #26a69a;
  }
  .submit-btn {
    align-self: flex-end;
    margin-top: 25px;
    padding: 10px 28px;
    background: #26a69a;
    border: none;
    border-radius: 8px;
    color: #004d40;
    font-weight: 700;
    font-size: 1.1em;
    cursor: pointer;
    transition: background-color 0.3s ease;
  }
  .submit-btn:hover {
    background-color: #4db6ac;
  }
  .login-title {
    margin-top: 30px;
    text-align: center;
    font-size: 1.8em;
    font-weight: 700;
    color: #b2dfdb;
    letter-spacing: 1.1px;
    user-select: none;
  }
</style>
</head>
<body>
    <div class="login-container">
    <form method="POST">
        <div class="form-group">
        <label for="username">Username</label>
        <input id="username" type="text" name="username" autocomplete="username" required />
        </div>
        <div class="form-group">
        <label for="password">Password</label>
        <input id="password" type="password" name="password" autocomplete="current-password" required />
        </div>
        <button class="submit-btn" type="submit">Login</button>
    </form>
    <div class="login-title">🔐 Login Portal</div>
    </div>
</body>
</html>
"""


def styled_message_page(title, message):
    return f"""<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8" />
<title>{title}</title>
<style>
  body {{
    background: linear-gradient(135deg, #004d40 0%, #00695c 100%);
    font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
    color: #e0f2f1;
    margin: 0;
    height: 100vh;
    display: flex;
    justify-content: center;
    align-items: center;
  }}
  .message-box {{
    background: rgba(38, 50, 56, 0.9);
    padding: 30px 40px;
    border-radius: 12px;
    box-shadow: 0 6px 25px rgba(0, 0, 0, 0.6);
    max-width: 480px;
    text-align: center;
  }}
  h1 {{
    margin-bottom: 20px;
    font-weight: 700;
    font-size: 2em;
    color: #80cbc4;
  }}
  p {{
    font-size: 1.2em;
    line-height: 1.4;
  }}
</style>
</head>
<body>
  <div class="message-box">
    <h1>{title}</h1>
    <p>{message}</p>
  </div>
</body>
</html>
"""


def get_post_body():
    clen = os.environ.get('CONTENT_LENGTH', '')
    try:
        length = int(clen)
    except ValueError:
        length = 0
    return sys.stdin.read(length) if length > 0 else ''


def hash_password(password):
    return hashlib.sha256(password.encode('utf-8')).hexdigest()


def base64url_encode(data):
    if isinstance(data, str):
        data = data.encode('utf-8')
    return base64.urlsafe_b64encode(data).decode('utf-8').rstrip('=')


def base64url_decode(data):
    padding = '=' * (-len(data) % 4)
    return base64.urlsafe_b64decode(data + padding)


def token_create(username):
    payload = {'username': username}
    return base64url_encode(json.dumps(payload))


def token_parse(token):
    try:
        decoded = base64url_decode(token)
        return json.loads(decoded)
    except:
        return None


def store_token(username, password_hash, token):
    with open('LoginData.txt', 'a') as f:
        fcntl.flock(f.fileno(), fcntl.LOCK_EX)
        f.write(f"{username},{password_hash},{token}\n")
        fcntl.flock(f.fileno(), fcntl.LOCK_UN)


def file_read_lines_locked():
    try:
        with open("LoginData.txt", 'r') as f:
            fcntl.flock(f.fileno(), fcntl.LOCK_SH)
            lines = f.readlines()
            fcntl.flock(f.fileno(), fcntl.LOCK_UN)
            return [line.strip() for line in lines]
    except FileNotFoundError:
        return []


def validate_token(token):
    payload = token_parse(token)
    if not payload or 'username' not in payload:
        return None
    for line in file_read_lines_locked():
        parts = line.split(",", 2)
        if len(parts) == 3 and parts[2] == token:
            return parts
    return None


def extract_token_from_cookie(cookie_header):
    if not cookie_header:
        return None
    for part in cookie_header.split(';'):
        part = part.strip()
        if part.startswith('token='):
            return part[len('token='):]
    return None


def find_user_by_username(username):
    for line in file_read_lines_locked():
        parts = line.split(",", 2)
        if len(parts) == 3 and parts[0] == username:
            return parts
    return None


def is_alphanumeric(s):
    return s.isalnum()


def main():
    cookie_header = os.environ.get("HTTP_COOKIE", "")
    method = os.environ.get("REQUEST_METHOD", "GET").upper()

    if method == "POST":
        body = get_post_body()
        data = parse_qs(body)
        username = data.get("username", [""])[0]
        password = data.get("password", [""])[0]

        if not username or not password:
            send(styled_message_page("Login Failed", "Username and password are required."))
            return
        if not is_alphanumeric(username):
            send(styled_message_page("Invalid Username", "Username must be alphanumeric only."))
            return

        user_data = find_user_by_username(username)
        if user_data:
            uname, pwd_hash, token = user_data
            if hash_password(password) == pwd_hash:
                send(styled_message_page("Welcome Back", f"Welcome back, <strong>{uname}</strong>!"), set_cookie_value=token)
            else:
                send(styled_message_page("Incorrect Password", "Password is incorrect."))
        else:
            new_token = token_create(username)
            store_token(username, hash_password(password), new_token)
            send(styled_message_page("Account Created", f"Welcome, <strong>{username}</strong>! Your account has been created."), set_cookie_value=new_token)
        return

    # GET request
    token = extract_token_from_cookie(cookie_header)
    valid_user = validate_token(token)
    if valid_user:
        uname = valid_user[0]
        send(styled_message_page("Welcome", f"Welcome back, <strong>{uname}</strong>!"))
    else:
        send(get_login_page())


if __name__ == "__main__":
    main()
