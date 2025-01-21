#! /usr/bin/env python3
import os
import re
import sys
import argparse
import macro_parser
import struct

#"GD32"
SYS_MAGIC = 0x47443332
IMAGE_HEADER_SIZE = 0x20

vtor_align_re = re.compile(r"^.*\s*RE_VTOR_ALIGNMENT\s*(.*)")
offset_re = re.compile(r"^.*\s*RE_([0-9A-Z_]+)_OFFSET\s*(.*)")

mbl_ver_re = re.compile(r"^.*\s*RE_MBL_VERSION\s*(.*)")
img_ver_re = re.compile(r"^.*\s*RE_IMG_VERSION\s*(.*)")

class SysSet():
    def __init__(self):
        self.buf = bytearray()
        
    def generate(self, config):
        jstart = 0x2000106F
        #jstart = 0
        #print("jstart = ", jstart)
    
        # System Settings
        # =====================================================
        vtor_align = macro_parser.evaluate_macro(config, vtor_align_re, 0, 1)
        offsets = macro_parser.evaluate_macro(config, offset_re, 1, 2)

        mbl_ver = macro_parser.evaluate_macro(config, mbl_ver_re, 0, 1)
        img_ver = macro_parser.evaluate_macro(config, img_ver_re, 0, 1)
 
        mbl_offset = offsets["MBL"] + vtor_align - IMAGE_HEADER_SIZE
        img0_offset = offsets["IMG_0"] + vtor_align - IMAGE_HEADER_SIZE
        img1_offset = offsets["IMG_1"] + vtor_align - IMAGE_HEADER_SIZE
 
        # add parts of system settings
        fmt = ('<' +
           # type sys_setting struct {
            'I' +    # Rsvd0             uint32
            'I' +    # Rsvd1             uint32
            'I' +    # Magic             uint32
            'I' +    # Sys Set Offset    uint32
            'I' +    # MBL Offset        uint32
            'I' +    # Sys Status Offset uint32
            'I' +    # Img0 Offset       uint32
            'I' +    # Img1 Offset       uint32
            'I' +    # MBL Version       uint32
            'I' +    # Img Version       uint32
            'I' +    # Version Locked Flag  uint32
            'I'      # Flash Size        uint32
        ) # }

        buf = struct.pack(fmt,
                jstart,
                0,
                SYS_MAGIC,
                offsets["SYS_SET"],
                mbl_offset,
                offsets["SYS_STATUS"],
                img0_offset,
                img1_offset,
                mbl_ver,
                img_ver,
                0xFFFFFFFF,
                offsets["END"])  
        self.buf += buf
        
        # padding 0xFF
        padding = 0x1000 - len(self.buf)
        pbytes  = b'\xff' * padding
        self.buf += pbytes
        
def gen_sysset(args):
     sysset = SysSet()
     #if args.mbl is not None:
     #   with open(args.mbl, 'rb') as f:
     #      mbl = f.read()

     #sysset.generate(args.config, (mbl[0] | (mbl[1] << 8) | (mbl[2] << 16) | (mbl[3] << 24)))
     sysset.generate(args.config)
     with open(args.outfile, 'wb') as f:
           f.write(sysset.buf)
           
def gen_sysstatus(args):
     # System Status padding, temp for AN521 erase
     padding  = b'\xff' * 0x2000
     with open(args.outfile, 'wb') as f:
           f.write(padding)
        
def intparse(text):
    """Parse a command line argument as an integer.

    Accepts 0x and other prefixes to allow other bases to be used."""
    return int(text, 0)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-c', '--config', metavar='filename', required=True,
                          help='Location of the file that contains macros')
    parser.add_argument("outfile")
    parser.add_argument("-t", "--type", metavar='type', required=True,
                          help='SYS_SET / SYS_STATUS')
    #parser.add_argument("--mbl", metavar='filename', required=True)
    
    args = parser.parse_args()
    if args.type == 'SYS_SET':
        gen_sysset(args)
    elif args.type == 'SYS_STATUS':
        gen_sysstatus(args)
        
if __name__ == '__main__':
    main()
                