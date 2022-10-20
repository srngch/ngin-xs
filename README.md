Co-authored-by: srngch <srngch.dev@gmail.com>

# NGIN-XS

NGIN-XS is a simple, lightweight, and fast web server written in C++.

## Features

## File Structure

```

```

## Usage

## Flowchart

## Logs

### 2022-09-21

- Created the repository
- Added the README.md file
- Specified [Convention](https://github.com/srngch/ngin-xs/wiki)
- Studied concept of multiplexing and socket programming

### 2022-09-25

- Made simple server-client programs: [code](https://github.com/srngch/ngin-xs/tree/9309c3e0e98e2d3b5c8f3f20582ad97ef10812d1/example)
- Learned usage of socket programming functions
  - `socket`, `bind`, `listen`, `accept`, `send`, `recv`, `connect`
  - `htons`, `htonl`, `inet_addr`
  - header: `<sys/socket.h>`, `<netinet/in.h>`, `<netinet/ip.h>`

### 2022-09-26

- Made echo server-client programs: [code](https://github.com/srngch/ngin-xs/tree/c3c04ce095b1b02d5bd47fb7a59844b2ca0582e1/example)
- Made a multiplexing server program: [code](https://github.com/srngch/ngin-xs/tree/c6b55c5d99f3cf697c5fa0b18195dac41d578aab/example)
- Learned usage of multiplexing function `poll`

### 2022-09-28

- Fixed bug when multiple clients request connection: [code](https://github.com/srngch/ngin-xs/commit/59191f716e7169a5c5fd36d710b3b0417c2a0940)
  - Close socket and initialize fd & revents when POLLHUP event occurs
  - Removed while loop in recv()
- Fixed bug regarding to EADDRINUSE: [code](https://github.com/srngch/ngin-xs/commit/b6cc8b2da0165fbd7955a64daa59b6a69a5f0b47)
  - Added close(listen_socket) when error occurs
  - Added setsockopt() to reuse port already in use
- Specified classes
  - Member variables
  - Member functions

### 2022-09-29

- Made a simple server program: [Issue](https://github.com/srngch/ngin-xs/issues/1), [PR](https://github.com/srngch/ngin-xs/pull/2)

### 2022-10-01

- Made a request syntax error exception: [Issue](https://github.com/srngch/ngin-xs/issues/5), [PR](https://github.com/srngch/ngin-xs/pull/7)
- Implemented Response class: [Issue](https://github.com/srngch/ngin-xs/issues/6), [PR](https://github.com/srngch/ngin-xs/pull/8)

### 2022-10-02

- Implemented GET method for static files: [Issue](https://github.com/srngch/ngin-xs/issues/9), [PR](https://github.com/srngch/ngin-xs/pull/12)
- Implemented request validation: [Issue](https://github.com/srngch/ngin-xs/issues/10), [PR](https://github.com/srngch/ngin-xs/pull/11)

### 2022-10-03

- Implemented calling CGI program with GET: [Issue](https://github.com/srngch/ngin-xs/issues/15), [PR](https://github.com/srngch/ngin-xs/pull/16)

### 2022-10-10

- Implemented autoindex: [Issue](https://github.com/srngch/ngin-xs/issues/17), [PR](https://github.com/srngch/ngin-xs/pull/20)
- Implemented POST method: [Issue](https://github.com/srngch/ngin-xs/issues/14), [PR](https://github.com/srngch/ngin-xs/pull/22)
- Changed header data type: [Issue](https://github.com/srngch/ngin-xs/issues/24), [PR](https://github.com/srngch/ngin-xs/pull/26)
- [WIP] Parse configuration file: [Issue](https://github.com/srngch/ngin-xs/issues/18)

### 2022-10-12

- Implemented DELETE method: [Issue](https://github.com/srngch/ngin-xs/issues/21), [PR](https://github.com/srngch/ngin-xs/pull/28)
- [WIP] Parse configuration file: [Issue](https://github.com/srngch/ngin-xs/issues/18)

### 2022-10-13

- Handled chunked request: [Issue](https://github.com/srngch/ngin-xs/issues/30), [PR](https://github.com/srngch/ngin-xs/pull/33)
- [WIP] Parse configuration file: [Issue](https://github.com/srngch/ngin-xs/issues/18)

### 2022-10-14

- Parsed configuration file: [Issue](https://github.com/srngch/ngin-xs/issues/18), [PR](https://github.com/srngch/ngin-xs/pull/35)
- [WIP] Implement directory path redirection: [Issue](https://github.com/srngch/ngin-xs/issues/19)

### 2022-10-15

- Implemented directory path redirection: [Issue](https://github.com/srngch/ngin-xs/issues/19), [PR](https://github.com/srngch/ngin-xs/pull/36)
- [WIP] Get required values from config class: [Issue](https://github.com/srngch/ngin-xs/issues/34)
- [WIP] Implement python-cgi: [Issue](https://github.com/srngch/ngin-xs/issues/32)

### 2022-10-17

- [WIP] Implement python-cgi: [Issue](https://github.com/srngch/ngin-xs/issues/32)

### 2022-10-18

- Implemented python-cgi: [Issue](https://github.com/srngch/ngin-xs/issues/32), [PR](https://github.com/srngch/ngin-xs/pull/37)
- Updated function to set response header: [Issue](https://github.com/srngch/ngin-xs/issues/25), [PR](https://github.com/srngch/ngin-xs/pull/38)
- [WIP] Get required values from config class: [Issue](https://github.com/srngch/ngin-xs/issues/34)

### 2022-10-19

- Implemented CUI client: [Issue](https://github.com/srngch/ngin-xs/issues/41), [PR](https://github.com/srngch/ngin-xs/pull/43)
- Added multipart/form-data file upload to python-cgi: [Issue](https://github.com/srngch/ngin-xs/issues/31), [PR](https://github.com/srngch/ngin-xs/pull/46)
- Fixed infinite validation when request header is invalid: [Issue](https://github.com/srngch/ngin-xs/issues/45), [PR](https://github.com/srngch/ngin-xs/pull/48)
- Fixed CRLF issue when response binary file: [Issue](https://github.com/srngch/ngin-xs/issues/49), [PR](https://github.com/srngch/ngin-xs/pull/51)
- [WIP] Get required values from config class: [Issue](https://github.com/srngch/ngin-xs/issues/34)

### 2022-10-20

- Implemented handling Connection header: [Issue](https://github.com/srngch/ngin-xs/issues/47), [PR](https://github.com/srngch/ngin-xs/pull/52)
- Implemented Limiting client max body size: [Issue](https://github.com/srngch/ngin-xs/issues/50), [PR](https://github.com/srngch/ngin-xs/pull/53)
- Refactored exception: [Issue](https://github.com/srngch/ngin-xs/issues/54), [PR](https://github.com/srngch/ngin-xs/pull/55)
- [WIP] Get required values from config class: [Issue](https://github.com/srngch/ngin-xs/issues/34)
