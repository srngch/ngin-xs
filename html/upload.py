#!/usr/local/bin/python3

import cgi, os, sys
import datetime
import cgitb; cgitb.enable()

# cgi.test()

# A nested FieldStorage instance holds the file
form = cgi.FieldStorage()
content = form['content'].value

# Test if the file was uploaded
if content:
    # strip leading path from file name
    # to avoid directory traversal attacks
    fn = os.path.basename(datetime.datetime.now().strftime("%Y%m%d-%H%M%S"))
    file = open(os.getcwd() + '/html/upload/' + fn, 'w')
    file.write(content)
    message = 'The file "' + fn + '" was uploaded successfully'

else:
   message = 'No file was uploaded'

res_body = f"<html><body><p>{message}</p></body></html>"

print (res_body)
