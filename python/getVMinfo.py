#!/usr/bin/python
#-*- coding:utf-8 -*-
import sys
import requests
import json

#get params
#print "脚本名：", sys.argv[0]

#for i in range(1, len(sys.argv)):
#	print "参数", i , sys.argv[i]

#***********************get login res****************************
client = requests.session()
url = 'http://' + sys.argv[3]+'/auth/login/'
client.get(url, verify=False)
cs = client.cookies['csrftoken']
login_data = dict(username=sys.argv[1], password=sys.argv[2], csrfmiddlewaretoken=cs, next='/')
auth = client.post(url, data=login_data, headers=dict(Referer=url))
if '<Response [200]>' != str(auth):
        print "login failed"
        exit()
#*********************get user id*******************************
cs1 = client.cookies['csrftoken']
post_data = {'first':'1111', 'second':'2222'}
res = client.post('http://'+sys.argv[3]+'/api/VDI/test/', \
        data=json.dumps(post_data), \
        headers={'X-Requested-With':'XMLHttpRequest', \
        'Content-Type':'application/json', \
        'X-CSRFToken':cs1 \
        })
restext = res.text
if restext == '\"not logged in\"':
        print 'login failed'
else:
        print 'login success'
        print json.loads(restext)['first']


#******************get vm info**********************

res = client.get('http://'+sys.argv[3]+'/api/getVMinfo/'+sys.argv[4], \
	headers={'X-Requested-With' : 'XMLHttpRequest'})
print res.text



