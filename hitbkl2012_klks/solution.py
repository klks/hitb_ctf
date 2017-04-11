import socket
import sys

#Connect to host
HOST = 'localhost'
PORT = 26498              # Arbitrary non-privileged port

#client example
import socket
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect((HOST, PORT))

#Test SIGPIPE
#client_socket.close()

data = client_socket.recv(1024)
print data
#data = client_socket.recv(1024)
data = data.split("\n")         #Split into lines
data = data[1].split(" ")[2]    #Extract 2nd line, 3rd Word
#print data

"""
Algo in C++

int checksum = 0;
for(int i=0; i<strlen(cChallenge); i++)
{
    checksum += cChallenge[i];
    checksum += 0xBABE;
    checksum <<= 2;
}
    AABBCCDD = checksum
    reordered to
    DDBB-AACC
"""
checksum = 0
for d in data:
    checksum += ord(d)
    checksum += 0xBABE
    checksum = checksum << 2

s_checksum = "%.8X" % (checksum,)
if len(s_checksum) > 8:
    s_checksum = s_checksum[-8:]
#print "Checksum : " + s_checksum
r_checksum = s_checksum[4:-2] + s_checksum[2:-4] + "-" + s_checksum[6:] + s_checksum[:2]
#print "Checksum : " + r_checksum

client_socket.send(r_checksum)
#client_socket.send("A" * 4096)
data = client_socket.recv(1024)
print data

client_socket.close()
