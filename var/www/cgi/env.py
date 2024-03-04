#!/usr/bin/python3

import os
import cgi

print("<html>")

print("<head>")
print("<h2>Environment with PYTHON</h2><br>")
print("</head>")

print("<body>")
for param in os.environ.keys():
    print("<b>%20s</b>: %s<br>" % (param, os.environ[param]))
print("</body>")

print("</html>")