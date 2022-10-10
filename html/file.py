#!/usr/local/bin/python3

import cgi, os
import datetime
import cgitb; cgitb.enable()

## get environ content-length
content_length = int(os.environ.get('CONTENT_LENGTH'))

form = cgi.FieldStorage()
content = form['file']

# Test if the file was uploaded
if content:
    # strip leading path from file name
    # to avoid directory traversal attacks
    fn = os.path.basename(datetime.datetime.now().strftime("%Y%m%d-%H%M%S"))
    open(os.getcwd() + '/html/upload/' + fn, 'wb').write(content)
    message = 'The file "' + fn + '" was uploaded successfully'

else:
   message = 'No file was uploaded'

res_body = f"<html><body><p>{message}</p></body></html>"

print (res_body)
