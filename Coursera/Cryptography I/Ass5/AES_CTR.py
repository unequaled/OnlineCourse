#!/usr/bin/env python

from Crypto.Cipher import AES
from Crypto.Util import Counter

mode = AES.MODE_CTR
key = "36f18357be4dbd77f050515c73fcf9f2"
ciphertext = "69dda8455c7dd4254bf353b773304eec0ec7702330098ce7f7520d1cbbb20fc388d1b0adb5054dbd7370849dbf0b88d393f252e764f1f5f7ad97ef79d59ce29f5f51eeca32eabedd9afa9329"

iv = ciphertext[:32]
ctr = ctr=Counter.new(128, initial_value=int(iv,16))

ciphertext = ciphertext[32:].decode('hex')
decryptor = AES.new(key.decode('hex'), mode, counter=ctr)
plaintext = decryptor.decrypt(ciphertext)
print plaintext