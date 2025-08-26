Content-Type: text/html; charset=utf-8
Content-Length: 2254

<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<title>Login</title>
<style>
    body {
        background: linear-gradient(to bottom, #0b3d2e, #14532d, #0b3d2e);
        font-family: 'Segoe UI', sans-serif;
        color: #e0e0e0;
        padding: 40px;
        margin: 0;
    }
    .container {
        max-width: 500px;
        margin: auto;
        background: rgba(0, 0, 0, 0.25);
        padding: 30px;
        border-radius: 12px;
        box-shadow: 0 8px 32px rgba(0, 0, 0, 0.5);
        backdrop-filter: blur(10px);
        animation: fadeIn 0.6s ease-in-out;
        text-align: center;
    }
    h1 {
        color: #a5d6a7;
        margin-bottom: 20px;
        text-shadow: 0 0 10px rgba(165, 214, 167, 0.8);
        animation: slideDown 0.5s ease-in-out;
    }
    label {
        display: block;
        margin: 15px 0 10px;
        font-weight: bold;
        color: #c8e6c9;
    }
    input[type="text"],
    input[type="password"] {
        width: 100%;
        padding: 10px;
        border-radius: 6px;
        border: none;
        margin-top: 5px;
        box-sizing: border-box;
        background: rgba(255, 255, 255, 0.1);
        color: #fff;
        font-size: 1em;
    }
    input[type="submit"] {
        margin-top: 20px;
        padding: 10px 20px;
        background: #43a047;
        border: none;
        border-radius: 6px;
        color: #fff;
        font-weight: bold;
        cursor: pointer;
        transition: background 0.3s ease;
        font-size: 1em;
    }
    input[type="submit"]:hover {
        background: #4caf50;
    }

    @keyframes fadeIn {
        from { opacity: 0; }
        to { opacity: 1; }
    }
    @keyframes slideDown {
        from { transform: translateY(-20px); opacity: 0; }
        to { transform: translateY(0); opacity: 1; }
    }
</style>
</head>
<body>
    <div class="container">
        <h1>🔐 Login</h1>
        <form action="" method="POST">
            <label>Username:
                <input type="text" name="username" required>
            </label>
            <label>Password:
                <input type="password" name="password" required>
            </label>
            <input type="submit" value="Login">
        </form>
    </div>
</body>
</html>
