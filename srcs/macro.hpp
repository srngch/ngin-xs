#ifndef	__MACRO_HPP__
# define __MACRO_HPP__

// Boolean
#define FT_FALSE	0
#define FT_TRUE		1
#define FT_ERROR	-1
#define ft_bool		int

// pipe
#define FT_PIPEIN	1
#define FT_PIPEOUT	0

// Ngin-xs
#define CRLF			"\r\n"
#define EMPTY_LINE		"\r\n\r\n"

// Default Configuration File Setting
#define DEFAULT_HOST					"*"
#define DEFAULT_PORT					8000
#define DEFAULT_SERVER_NAME				"localhost"
#define DEFAULT_WEB_ROOT				"html"
#define DEFAULT_CLIENT_MAX_BODY_SIZE	1048576
#define DEFAULT_ERROR_PAGE_400			"error_pages/400.html"
#define DEFAULT_ERROR_PAGE_403			"error_pages/403.html"
#define DEFAULT_ERROR_PAGE_404			"error_pages/404.html"
#define DEFAULT_ERROR_PAGE_405			"error_pages/405.html"
#define DEFAULT_ERROR_PAGE_500			"error_pages/500.html"
#define DEFAULT_ERROR_PAGE_505			"error_pages/505.html"
#define DEFAULT_AUTO_INDEX				"off"
#define DEFAULT_ALLOWED_METHOD			"GET"
#define DEFAULT_INDEX					"index.html"

// Response Status
#define	HTTP_OK						"200 OK"
#define HTTP_CREATED				"201 CREATED"
#define HTTP_NO_CONTENT				"204 No Content"
#define HTTP_BAD_REQUEST			"400 Bad Request"
#define HTTP_FORBIDDEN				"403 Forbidden"
#define HTTP_NOT_FOUND				"404 Not Found"
#define HTTP_METHOD_NOT_ALLOWED		"405 Method Not Allowed"
#define HTTP_INTERNAL_SERVER_ERROR	"500 Internal Server Error"
#define HTTP_NOT_IMPLEMENTED		"501 Not Implemented"
#define HTTP_VERSION_NOT_SUPPORTED	"505 HTTP Version Not Supported"

#define ERROR_PAGES_PATH	"./error_pages/"
#define WEB_ROOT			"./html"  // TODO: remove this after implement Config class

#define PYTHON_PATH "/opt/homebrew/bin/python3"
#define	CGI_INFILE "./infile.tmp"
#define	CGI_OUTFILE "./outfile.tmp"
#define CGI_FILE_MODE 0644
#define CGI_READ_BUF_SIZE 1024

#endif
