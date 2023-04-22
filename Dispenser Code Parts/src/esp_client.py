import socket
import nmap

def find_local_ip():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect(('8.8.8.8', 1))  # connect() for UDP doesn't send packets
    local_ip_address = s.getsockname()[0]
    ip_search = local_ip_address.split(".")[:-1]
    ip_range = ""
    for item in ip_search:
        ip_range += item
        ip_range += "."
    ip_range += "1/24"
    s.close()
    return ip_range


def find_disp_ip(ip_range):
    nm = nmap.PortScanner()
    data = nm.scan(hosts=ip_range, arguments="-sN").get('scan')

    for item in data:
        try:
            if nm[item]['addresses']['mac'] == "80:7d:3a:fc:f0:80":
                print(item)
                return item
        except:
            pass


    


sock = socket.socket()

ip_range = find_local_ip()
#host = find_disp_ip(ip_range) #ESP32 IP in local network
host = "192.168.34.163"
port = 80             #ESP32 Server Port    


sock.connect((host, port))
print("connected")

#initmsg = "start".encode()
#sock.send(initmsg)

while(True):
    data = ""       
    
    while len(data) < 1:
        data += sock.recv(1).decode()

    print(data)

sock.close()



