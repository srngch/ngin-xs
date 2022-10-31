#!/usr/bin/env python3

import cgi, os, sys, io
import datetime
import cgitb

cgitb.enable()
from mimetypes import guess_extension

WEB_UPLOAD = "./html/upload"
if not os.path.exists(WEB_UPLOAD):
    os.makedirs(WEB_UPLOAD)


def save_file(data, basename):
    if not data or not basename:
        return
    with open(os.getcwd() + "/html/upload/" + basename, "wb") as f:
        f.write(data)


try:
    # get environment variables
    path_info = os.environ.get("PATH_INFO")
    request_method = os.environ.get("REQUEST_METHOD")
    request_uri = os.environ.get("REQUEST_URI")
    content_type = os.environ.get("CONTENT_TYPE")
    content_length = int(os.environ.get("CONTENT_LENGTH"))
    query_stirng = os.environ.get("QUERY_STRING")
    script_name = os.environ.get("SCRIPT_NAME")

    buf = ""
    buf = sys.stdin.buffer.read()

    status = ""

    if request_method == "POST" and script_name == "/directory/youpi.bla":
        if buf:
            fn = os.path.basename(datetime.datetime.now().strftime("%Y%m%d-%H%M%S"))
            extension = guess_extension(content_type.partition(";")[0].strip())
            save_file(buf, f"{fn}{extension}")
            message = f"""The file <a href="/upload/{fn}">upload/{fn}</a> was uploaded successfully"""
            if content_type.startswith("image/"):
                img_tag = f'<img src="/upload/{fn}" alt="uploaded image"/>'
        else:
            message = "No file was uploaded"

    """
    Binary file upload: POST upload.py
    """
    message = ""
    img_tag = ""
    if request_method == "POST" and script_name == "/binary.py":
        if buf:
            fn = os.path.basename(datetime.datetime.now().strftime("%Y%m%d-%H%M%S"))
            extension = guess_extension(content_type.partition(";")[0].strip())
            save_file(buf, f"{fn}{extension}")
            message = f"""The file <a href="/upload/{fn}">upload/{fn}</a> was uploaded successfully"""
            if content_type.startswith("image/"):
                img_tag = f'<img src="/upload/{fn}" alt="uploaded image"/>'
            status = "201 CREATED"
        else:
            message = "No file was uploaded"
            status = "200 OK"

    """
    multipart/form-data: POST form.py
    """
    formData = ""
    if request_method == "POST" and script_name == "/multipart.py":
        formData += "<h3>Form</h3><ul>"
        ctype, pdict = cgi.parse_header(content_type)

        if ctype == "multipart/form-data":
            form = cgi.FieldStorage(
                fp=io.BytesIO(buf), environ=os.environ, keep_blank_values=True
            )
            for key in form.keys():
                if type(form[key].value) == bytes:
                    filename = form[key].filename
                    data = form[key].file.read()
                    if not filename:
                        d = f'File(empty)'
                    else:
                        save_file(data, filename)
                        d = f'File(<a href="/upload/{filename}">upload/{filename}</a>)'
                        status = "201 CREATED"
                else:
                    d = form[key].value
                formData += f"<li>{key}: {d}</li>"
        formData += "</ul>"
        message = "The form data was sent successfully"

    body = f"""<html>
    <head>
        <title>Python CGI</title>
        <meta charset="utf-8">
    </head>
    <body>
        <h1>python-cgi: {request_uri}</h1>
        <p>{message}</p>
        <ul>
            <li>path_info: {path_info}</li>
            <li>request_method: {request_method}</li>
            <li>content_type: {content_type}</li>
            <li>content_length: {content_length}</li>
        </ul>
        {img_tag}
        {formData}
    </body>
    </html>"""

    if status == "":
        status = "200 OK"

    print(f"Status: {status}")
    print("Content-type: text/html")
    print(f"Content-Length: {len(body)}")
    print()
    print(body)

except Exception as e:
    print(e)
