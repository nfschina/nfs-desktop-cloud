#!/usr/bin/python
#-*- coding:utf-8 -*-

import base64
import re
import sys

from Crypto.Cipher import AES

def encrypt( text):
    key = 'd41d8cd98f00b204'
    mode = AES.MODE_ECB
    bs = 16
    PADDING = lambda s: s + (bs - len(s) % bs) * chr(bs - len(s) % bs)

    generator = AES.new(key, mode)  # ECB模式无需向量iv
    try:
        crypt = generator.encrypt(PADDING(text))
        crypted_str = base64.b64encode(crypt)
        result = crypted_str.decode()
    except Exception:
        result = '加密失败，请确认输入是否有误!'
    return result

def decrypt(text):
    key = 'd41d8cd98f00b204'
    mode = AES.MODE_ECB

    generator = AES.new(key,mode)  # ECB模式无需向量iv
    text += (len(text) % 4) * '='
    try:
        decrpyt_bytes = base64.b64decode(text)
        meg = generator.decrypt(decrpyt_bytes)
        # 去除解码后的非法字符
        result = re.compile('[\\x00-\\x08\\x0b-\\x0c\\x0e-\\x1f\n\r\t]').sub('', meg.decode())
    except Exception:
        result = '解密失败，请确认输入是否有误!'
    return result

print decrypt(sys.argv[1])
