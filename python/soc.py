#!/usr/bin/python
#-*- coding:utf-8 -*-
import sys
import socket
PORT=59000
s=socket.socket(socket.AF_INET,socket.SOCK_STREAM)      #定义socket类型，网络通信，TCP
s.connect((sys.argv[1],PORT))       #要连接的IP与端口
res = ''
while True:
	data=s.recv(1024)     #把接收的数据定义为变量
	res = res + data
	if ( len(data) < 1 ) :
		break
	#print data         #输出变量
print res
s.close()   #关闭连接
