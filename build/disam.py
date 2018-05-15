#!/usr/bin/env python
# -*- coding: utf-8 -*-

import struct
import sys
import os
import subprocess
import re

objdump_cmd="objdump"
objdump_param = ('-color', '-macho' ,'-source' ,'-mcpu=haswell' ,'-section=__const' ,'-section=__cstring' ,'-triple=x86_64-apple-darwin17.5.0')

rip_re = re.compile('(\d+)[(][%]rip[)]')
def sd_to_string(value):
    hex_v = ''.join(x for x in value)
    double_value = struct.unpack('>d', struct.pack('Q', int(hex_v,16)))
    string_value =  '%.7f' % double_value
    return string_value

def fine_vmovsd(lines, addr):

    for line in lines:
        dt = line.split('\t')
        if len(dt)<2:
            continue
        try:
            d_v = dt[0].replace(':','')
            base_addr = int(d_v,16)
#            if base_addr == 0x10000f2a0:
#                value = dt[1].split(' ')[:8]
#                hex_v = ''.join(x for x in value)
#                print base_addr==addr, hex_v,sd_to_string(value)
            #print '0=',dt[0], base_addr, addr
            if base_addr == addr:
                value = dt[1].split(' ')[:8]
                #print value
                return sd_to_string(value)
            elif base_addr + 8 ==  addr:
                value = dt[1].split(' ')[8:16]
                #print value
                return sd_to_string(value)
        except:
            continue
    print('not find ', hex(addr))
    return hex(addr)




def disam(app_path):

    params = list()
    params.append(objdump_cmd)
    params.extend(objdump_param)
    params.append(app_path)


    child = subprocess.Popen(params, stdout=subprocess.PIPE)
    asm_source = child.communicate()[0]
    with open(app_path+".o.asm", 'w') as f:
        f.write(asm_source)
        f.close()
    #print asm_source
    fine_source=""
    lines = asm_source.split('\n')
#    for line in lines:
#        print line
    for line in lines:

        rel_pos = 0
        result = rip_re.search(line)
        if result:
            rel_pos = int(result.groups()[0])
            dt=line.split('\t')
            ins_len = len(dt[1].strip().split(' '))
            base_addr = int(dt[0][:-1],16)
            value_addr = rel_pos+base_addr+ins_len
            v_result = hex(value_addr)
            if line.count('vmovsd') :
                #print value_addr, v_result
                v_result = fine_vmovsd(lines, value_addr)
            fine_source += line[:result.start()] + v_result + line[result.end():] +"\n"
        else:
            fine_source += line + "\n"
    return fine_source



if __name__ == "__main__":
    if len(sys.argv) == 1:
        print("""usage: disam <your app>\n""")
        exit(0)
    app = sys.argv[1]
    app_path = os.path.abspath(app)
    print(app)
    asm_source = disam(app_path)
    asm_file = app_path + ".asm"
    with open(asm_file, 'w') as f:
        f.write(asm_source)
        f.close()
