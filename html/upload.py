import cgi, os
import datetime
import cgitb; cgitb.enable()

# keys = os.environ.keys()

# for item in keys:
#   print (f"{item}={os.environ[item]}")

form = cgi.FieldStorage()
print(form)
print("~~~~~~~~~~~~~~")
print(form.keys())
print("~~~~~~~~~~~~~~")

# A nested FieldStorage instance holds the file
content = form['content'].value

print("@@@@@@@@@@@")
print(content)
print("@@@@@@@@@@@")

# Test if the file was uploaded
if content:
    # strip leading path from file name
    # to avoid directory traversal attacks
    fn = os.path.basename(datetime.datetime.now().strftime("%Y%m%d-%H%M%S"))
    open('/Users/hhkim/ngin-xs/html/upload/' + fn, 'w').write(content)
    message = 'The file "' + fn + '" was uploaded successfully'

else:
    message = 'No file was uploaded'

res_body = f"<html><body><p>{message}</p></body></html>"

print (res_body)
