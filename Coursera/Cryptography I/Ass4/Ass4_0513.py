import urllib2
import sys

TARGET = 'http://crypto-class.appspot.com/po?er='
#--------------------------------------------------------------
# padding oracle
#--------------------------------------------------------------
class PaddingOracle(object):
    def query(self, q):
        target = TARGET + urllib2.quote(q)    # Create query URL        
        req = urllib2.Request(target)         # Send HTTP request to server
        try:
            f = urllib2.urlopen(req)          # Wait for response
        except urllib2.HTTPError, e:          
            print "We got: %d" % e.code       # Print response code
            if e.code == 404:
                return True # good padding
            return False # bad padding
        
def strxor(a, b):     # xor two strings of different lengths
    if len(a) > len(b):
        return "".join([chr(ord(x) ^ ord(y)) for (x, y) in zip(a[:len(b)], b)])
    else:
        return "".join([chr(ord(x) ^ ord(y)) for (x, y) in zip(a, b[:len(a)])])

def padding(a):
    if a == 1:
        return "01"
    if a == 2:
        return "0202"
    if a == 3:
        return "030303"
    if a == 4:
        return "04040404"
    if a == 5:
        return "0505050505"
    if a == 6:
        return "060606060606"
    if a == 7:
        return "07070707070707"
    if a == 8:
        return "0808080808080808"
    if a == 9:
        return "090909090909090909"
    if a == 10:
        return "0a0a0a0a0a0a0a0a0a0a"
    if a == 11:
        return "0b0b0b0b0b0b0b0b0b0b0b"
    if a == 12:
        return "0c0c0c0c0c0c0c0c0c0c0c0c"
    if a == 13:
        return "0d0d0d0d0d0d0d0d0d0d0d0d0d"
    if a == 14:
        return "0e0e0e0e0e0e0e0e0e0e0e0e0e0e"
    if a == 15:
        return "0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f"
    if a == 16:
        return "10101010101010101010101010101010"

if __name__ == "__main__":
    cipher_iv = "f20bdba6ff29eed7b046d1df9fb70000"
    cipher_c0 = "58b1ffb4210a580f748b4ac714c001bd"
    cipher_c1 = "4a61044426fb515dad3f21f18aa577c0"
    cipher_c2 = "bdf302936266926ff37dbf7035d5eeb4"
    sec = ""
    m1 = "6172652053717565616d697368204f73"
    turn = 1
    po = PaddingOracle()
    
    x = 30
    while turn < 17:
        for i in range(256):
            temp =  strxor(cipher_c0[x:].decode('hex'), padding(turn).decode('hex') )
            if i < 16:
                sec = ('0' + hex(i)[2:]) + m1
                temp = strxor( sec, temp).encode('hex')
            else:
                sec = (hex(i)[2:]) + m1
                temp = strxor( sec.decode('hex'), temp).encode('hex')
            temp = cipher_c0[:x] + temp
            #print s + cipher_iv + temp + cipher_c1        
            if po.query(cipher_iv + temp + cipher_c1):
                m1 = sec
                print m1
                break
        x -= 2    
        turn += 1
        
    '''    
    po = PaddingOracle()
    xyz = cipher_iv+ cipher_c0[:30] + strxor(strxor(m1.decode('hex'),cipher_c0[30:].decode('hex')),padding(1).decode('hex')).encode('hex') + cipher_c1
    print xyz
    po.query( xyz)       # Issue HTTP query with the given argument
    '''