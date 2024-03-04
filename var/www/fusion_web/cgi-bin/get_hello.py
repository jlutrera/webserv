#!/usr/bin/python3

import sys
import os
from urllib.parse import parse_qs

# Create instance of FieldStorage
form = parse_qs(os.environ.get("QUERY_STRING", ""), keep_blank_values=True)

# Get data from fields
first_name = form.get('first_name', [None])[0]
last_name = form.get('last_name', [None])[0]

# Debugging: print the raw query string and environment variables

print("<html>")
print("<head>")
print("<title>Hello - Second CGI Program</title>")
print("<html>")
print("<head>")
print("<h2>Hello %s %s</h2>" % (first_name, last_name))
print("</body>")
print("</html>")
