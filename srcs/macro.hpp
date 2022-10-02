#ifndef	__MACRO_HPP__
# define __MACRO_HPP__

// Boolean
#define FT_FALSE	0
#define FT_TRUE		1
#define FT_ERROR	-1
#define ft_bool		int

// Ngin-xs
#define CRLF		"\r\n"

// Response Status
#define	HTTP_OK						"200 OK"
#define HTTP_BAD_REQUEST			"400 Bad Request"
#define HTTP_NOT_FOUND				"404 Not Found"
#define HTTP_METHOD_NOT_ALLOWED		"405 Method Not Allowed"
#define HTTP_VERSION_NOT_SUPPORTED	"505 HTTP Version Not Supported"

#define ERROR_PAGES_PATH	"./error_pages/"
#define WEB_ROOT			"./html"  // TODO: remove this after implement Config class

#endif
