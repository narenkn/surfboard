# UDP client example
import socket
client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
data = """ <data> <user>naren</user> <debug>1</debug> </data> """;
client_socket.sendto(data, ("idcas003",9600))
client_socket.close()
