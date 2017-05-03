#!/usr/bin/python

#Wheespa Client test script
from socket import *
import time
import ssl
import os

host = "localhost"
port = 8890
username = "root\n"

fd = socket(AF_INET, SOCK_STREAM)

# print "connecting to {}...".format(host)
# print "[+] Choose option."
# print "  [-] wheespa_login."
# print "  [-] wheespa_create_account."
# print
# choice = input("[+] Enter choice: ")
choice = 1
# fd.connect((host, port))
# 
# fd.send("wheespa_login")
# stop = False
# cnt = 0
# while not stop:
#     username = raw_input("  [-] username: ")
#     password = raw_input("  [-] password: ")
#     fd.send(username)
#     time.sleep(0.1)
#     fd.send(password)
#     print fd.recv(1024),1
#     lst = fd.recv(1024)
#     
#     if lst == "" or cnt == 2:
#         print "Connection closed!"
#         stop = True
#     else:
#         print lst
#         
#     cnt += 1



ssl_obj = ssl.SSLSocket(fd, ssl_version=ssl.PROTOCOL_TLSv1)
sslcc = ssl_obj.connect((host, port))

if choice == 1:
    for i in range(10):
        ssl_obj.send("wheespa_login\n")
    ssl_obj.send("--end--")
    stop = False
    cnt = 0
    # while not stop:
    #     username = raw_input("  [-] username: ")
    #     password = raw_input("  [-] password: ")
    #     ssl_obj.send(username)
    #     time.sleep(0.1)
    #     ssl_obj.send(password)
    #     print ssl_obj.recv(1024),
    #     lst = ssl_obj.recv(1024)
    #     
    #     if lst == "" or cnt == 2:
    #         print "Connection closed!"
    #         stop = True
    #     else:
    #         print lst
    #         
    #     cnt += 1
else:
    ssl_obj.send("wheespa_create_account")
    stop = False
    cnt = 0
    while not stop:
        username = raw_input("  [-] username: ")
        password0 = raw_input("  [-] password: ")
        password1 = raw_input("  [-] password: ")
        ssl_obj.send(username)
        time.sleep(0.1)
        ssl_obj.send(password0)
        time.sleep(0.1)
        ssl_obj.send(password1)
        print ssl_obj.recv(1024),
        lst = ssl_obj.recv(1024)
        
        if lst == "" or cnt == 2:
            print "Connection closed!"
            stop = True
        else:
            print lst
            
        cnt += 1

fd.close()

#C03A7A3659B7ED6C85B1AEA80C6CA59539DB26D8
