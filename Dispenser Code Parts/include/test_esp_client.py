from time import sleep
import socket               
 
sock = socket.socket()
 
host = "192.168.245.23" #ESP32 IP in local network
port = 80             #ESP32 Server Port    
sock.connect((host, port))

print("success")

while(True):
    
    message = "Hello World\n"
    sock.send(message.encode())
 
    data = ""       
 
    while len(data) < len(message):
        data += sock.recv(1).decode()
 
    print(data)
    sleep(5)
 
sock.close()
