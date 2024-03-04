#!/usr/bin/python3

import datetime
import cgi

print("<html>")
print("<head>")
print(datetime.datetime.strftime(datetime.datetime.now(), "<h1>  %H:%M:%S </h1>"))
print("</head>")
print("</html>")