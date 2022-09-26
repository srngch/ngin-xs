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
