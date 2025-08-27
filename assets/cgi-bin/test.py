import os

# Replace with the URL you want to curl; assuming the script is served on localhost:8000
url = "http://localhost:8090/cgi-bin/test.py"

# Run: curl -i <url> > /dev/null 2>&1
os.system(f"curl -i {url} > /dev/null")
