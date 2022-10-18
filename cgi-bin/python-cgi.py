#!/usr/bin/env python3

import cgi, os, sys
import datetime
import cgitb
from mimetypes import guess_extension
from urllib import request; cgitb.enable()

try:
    # get environment variables
    path_info = os.environ.get('PATH_INFO')
    request_method = os.environ.get('REQUEST_METHOD')
    request_uri = os.environ.get('REQUEST_URI')
    content_type = os.environ.get('CONTENT_TYPE')
    content_length = int(os.environ.get('CONTENT_LENGTH'))

    buf = sys.stdin.buffer.read()
    # cgi.test()


    """
    File upload: POST upload.py
    """
    message = ""
    if (request_method == 'POST' and request_uri == '/upload.py'):
        if buf:
            fn = os.path.basename(datetime.datetime.now().strftime("%Y%m%d-%H%M%S"))
            extension = guess_extension(content_type.partition(';')[0].strip())
            fn += extension
            file = open(os.getcwd() + '/html/upload/' + fn, 'wb')
            file.write(buf)
            message = f'''The file <a href="/upload/{fn}">upload/{fn}</a> was uploaded successfully'''

        else:
            message = 'No file was uploaded'

    body = f'''<html>
    <head>
        <title>Python CGI</title>
        <meta charset="utf-8">
    </head>
    <body>
        <h1>python-cgi: {request_uri}</h1>
        <h3>buf</h3>
        <pre>{buf}</pre>
        <ul>
            <li>path_info: {path_info}</li>
            <li>request_method: {request_method}</li>
            <li>content_type: {content_type}</li>
            <li>content_length: {content_length}</li>
            <li>message: {message}</li>
        </ul>
    </body>
    </html>'''


    print("Content-type: text/html")
    print()
    print(body)

except Exception as e:
    print(e)
