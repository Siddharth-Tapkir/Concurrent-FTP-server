# Concurrent FTP Server

A multi-client FTP server developed in C using TCP socket programming and POSIX threads. 
The server allows multiple clients to connect simultaneously and perform file transfer operations.

## Features

- Multi-client support using POSIX threads (pthreads)
- TCP-based client-server communication
- File upload and download
- Concurrent handling of multiple client connections
- Client-server communication using sockets
- Linux/UNIX system programming concepts

## Technologies Used

- C
- TCP/IP
- Socket Programming
- POSIX Threads (pthreads)
- Linux/UNIX System Programming

## Architecture

The server follows a client-server architecture:

Client 1 ──┐
Client 2 ──┼──> FTP Server
Client 3 ──┘       │
                   ├── Thread 1
                   ├── Thread 2
                   └── Thread 3

Each connected client is handled by a separate thread, allowing multiple clients to communicate with the server concurrently.

## How to Compile

```bash
gcc server.c -o server -pthread
gcc client.c -o client
