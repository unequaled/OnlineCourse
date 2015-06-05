# This script reads a file and displays its size and checksums
def get_custom_checksum(input_file_name):
    from datetime import datetime
    starttime = datetime.now()
    # START: Actual checksum calculation
    from hashlib import md5, sha1, sha224, sha384, sha256, sha512
    #chunk_size = 1 # 1 byte -- NOT RECOMENDED -- USE AT LEAST 1KB. When 1KB takes 1 min to run, 1B takes 19 minutes to run
    #chunk_size = 1024 # 1 KB
    chunk_size = 1048576 # 1024 B * 1024 B = 1048576 B = 1 MB
    file_md5_checksum = md5()
    file_sha1_checksum = sha1()
    file_sha224_checksum = sha224()
    file_sha256_checksum = sha256()
    file_sha384_checksum = sha384()
    file_sha512_checksum = sha512()
    try:
        with open(input_file_name, "rb") as f:
            byte = f.read(chunk_size)
            previous_byte = byte
            byte_size = len(byte)
            file_read_iterations = 1
            while byte:
                file_md5_checksum.update(byte)
                file_sha1_checksum.update(byte)
                file_sha224_checksum.update(byte)
                file_sha256_checksum.update(byte)
                file_sha384_checksum.update(byte)
                file_sha512_checksum.update(byte)
                previous_byte = byte
                byte = f.read(chunk_size)
                byte_size += len(byte)
                file_read_iterations += 1
    except IOError:
        print ('File could not be opened: %s' % (input_file_name))
        #exit()
        return
    except:
        raise
    # END: Actual checksum calculation
    # For storage purposes, 1024 bytes = 1 kilobyte
    # For data transfer purposes, 1000 bits = 1 kilobit
    kilo_byte_size = byte_size/1024
    mega_byte_size = kilo_byte_size/1024
    giga_byte_size = mega_byte_size/1024
    bit_size = byte_size*8
    kilo_bit_size = bit_size/1000
    mega_bit_size = kilo_bit_size/1000
    giga_bit_size = mega_bit_size/1000
    last_chunk_size = len(previous_byte)
    stoptime = datetime.now()
    processtime = stoptime-starttime
    custom_checksum_profile = {
        'starttime': starttime,
        'byte_size': byte_size,
        'kilo_byte_size': kilo_byte_size,
        'mega_byte_size': mega_byte_size,
        'giga_byte_size': giga_byte_size,
        'bit_size': bit_size,
        'kilo_bit_size': kilo_bit_size,
        'mega_bit_size': mega_bit_size,
        'giga_bit_size': giga_bit_size,
        'file_read_iterations': file_read_iterations,
        'last_chunk_size': last_chunk_size,
        'md5_checksum': file_md5_checksum.hexdigest(),
        'sha1_checksum': file_sha1_checksum.hexdigest(),
        'sha224_checksum': file_sha224_checksum.hexdigest(),
        'sha256_checksum': file_sha256_checksum.hexdigest(),
        'sha384_checksum': file_sha384_checksum.hexdigest(),
        'sha512_checksum': file_sha512_checksum.hexdigest(),
        'stoptime': stoptime,
        'processtime': processtime,
        }
    return custom_checksum_profile

    # Thanks to the following resources for helping with this function
    # http://stackoverflow.com/questions/1035340/reading-binary-file-in-python
    # http://abstracthack.wordpress.com/2007/10/19/calculating-md5-checksum/
    # http://www.speedguide.net/conversion.php

def print_custom_checksum(input_file_name):
    custom_checksum_profile = get_custom_checksum(input_file_name)
    try:
        print ('Start Time            :', custom_checksum_profile['starttime'])
        print ('File Size (bytes)     :', custom_checksum_profile['byte_size'])
        print ('File Size (kilobytes) :', custom_checksum_profile['kilo_byte_size'])
        print ('File Size (megabytes) :', custom_checksum_profile['mega_byte_size'])
        print ('File Size (gigabytes) :', custom_checksum_profile['giga_byte_size'])
        print ('File Size (bits)      :', custom_checksum_profile['bit_size'])
        print ('File Size (kilobits)  :', custom_checksum_profile['kilo_bit_size'])
        print ('File Size (megabits)  :', custom_checksum_profile['mega_bit_size'])
        print ('File Size (gigabits)  :', custom_checksum_profile['giga_bit_size'])
        print ('File Read Iterations  :', custom_checksum_profile['file_read_iterations'])
        print ('Last Chunk (bytes)    :', custom_checksum_profile['last_chunk_size'])
        print ('MD5                   :', custom_checksum_profile['md5_checksum'])
        print ('SHA1                  :', custom_checksum_profile['sha1_checksum'])
        print ('SHA224                :', custom_checksum_profile['sha224_checksum'])
        print ('SHA256                :', custom_checksum_profile['sha256_checksum'])
        print ('SHA384                :', custom_checksum_profile['sha384_checksum'])
        print ('SHA512                :', custom_checksum_profile['sha512_checksum'])
        print ('Stop Time             :', custom_checksum_profile['stoptime'])
        print ('Processing Time       :', custom_checksum_profile['processtime'])
    except TypeError: #  'NoneType' object is not subscriptable --- basically this should happen when the input file could not be opened
        #raise
        pass
    # csv output

file_name = "C:\Users\IBM_ADMIN\Downloads\1.mp4"
print ('Processing File       :', file_name)
print_custom_checksum(file_name)

# Thanks to the following resources for helping with argparse
# http://www.doughellmann.com/PyMOTW/argparse/