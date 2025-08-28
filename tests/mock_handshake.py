import socket

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)

sock.connect(("127.0.0.1", 1689))

sock.send(b"0")

data = sock.recv(1024).decode()

if data == "1": # "go ahead" signal
    sock.send(b"Hello, sock!")

print("RECEIVED: " + sock.recv(1024).decode())

sock.close()
