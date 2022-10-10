#!/usr/local/bin/python3

import os, sys
import cgitb; cgitb.enable()

content_length = int(os.environ.get('CONTENT_LENGTH'))
buf = sys.stdin.buffer.read(content_length)

html = f"<html><body><h1>readstdin.py</h1><p>content_length: {content_length}</p><p>stdin: {buf}</p>sys.stdin.buffer: <p></p></body></html>"

print (html)
