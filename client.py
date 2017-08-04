#!/usr/bin/python

#Wheespa Client test script
from socket import *
import time
import ssl
import os, sys
import select

host = "localhost"
port = 2748 

i = 0

fd = socket(AF_INET, SOCK_STREAM)
ssl_obj = ssl.SSLSocket(fd, ssl_version=ssl.PROTOCOL_TLSv1)
ssl_obj.connect((host, port))
print 'write'
if int(sys.argv[1]) == 1:
    ssl_obj.write('WHEESPA_LOGIN')
    time.sleep(1)
    ssl_obj.write('wells\n')
    time.sleep(1)
    ssl_obj.write('passwd\n')
    print ssl_obj.read(32)
    
elif int(sys.argv[1]) == 2:
    ssl_obj.write('WHEESPA_ADMIN_LOGIN')
    time.sleep(1)
    ssl_obj.write('wAdministrator\n')
    time.sleep(1)
    ssl_obj.write('wh33spaAdm1n\n')
    print ssl_obj.read(32)
else:
    sys.exit(1)
    
msg = ''
while msg.strip() != 'exit':
    ready = select.select([ssl_obj,], [], [], 2)
    while ready[0]:
        recvd = ssl_obj.read(1024)
        if recvd is '': break
        print recvd
        ready = select.select([ssl_obj, ], [], [], 1)
    msg = raw_input()
    ssl_obj.write(msg)


print "closing..."
ssl_obj.close()
fd.close()
