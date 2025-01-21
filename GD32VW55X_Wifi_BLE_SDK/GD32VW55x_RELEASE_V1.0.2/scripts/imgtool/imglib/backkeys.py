# Copyright 2017 Linaro Limited
# Copyright (c) 2017-2019, Arm Limited.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""
Cryptographic key management for imgtool.
"""

from __future__ import print_function
from Crypto.Hash import SHA256
from Crypto.PublicKey import RSA
from Crypto.Signature import PKCS1_v1_5, PKCS1_PSS
import hashlib
from pyasn1.type import namedtype, univ
from pyasn1.codec.der.encoder import encode

# Sizes that bootutil will recognize
RSA_KEY_SIZES = [2048, 3072]

# By default, we use RSA-PSS (PKCS 2.1).  That can be overridden on
# the command line to support the older (less secure) PKCS1.5
sign_rsa_pss = True

AUTOGEN_MESSAGE = "/* Autogenerated by imgtool.py, do not edit. */"

class RSAUsageError(Exception):
    pass

class RSAPublicKey(univ.Sequence):
    componentType = namedtype.NamedTypes(
            namedtype.NamedType('modulus', univ.Integer()),
            namedtype.NamedType('publicExponent', univ.Integer()))

class RSAutil():
    def __init__(self, key):
        """Construct an RSA key with the given key data"""
        self.key = key

    def key_size(self):
        return self.key.n.bit_length()

    @staticmethod
    def generate(key_size=2048):
        if key_size not in RSA_KEY_SIZES:
            raise RSAUsageError("Key size {} is not supported by MCUboot"
                                .format(key_size))
        return RSAutil(RSA.generate(key_size))

    def export_private(self, path):
        with open(path, 'wb') as f:
            f.write(self.key.exportKey('PEM'))

    def get_public_bytes(self):
        node = RSAPublicKey()
        node['modulus'] = self.key.n
        node['publicExponent'] = self.key.e
        return bytearray(encode(node))

    def emit_c(self):
        print(AUTOGEN_MESSAGE)
        print("const unsigned char rsa_pub_key[] = {", end='')
        encoded = self.get_public_bytes()
        for count, b in enumerate(encoded):
            if count % 8 == 0:
                print("\n\t", end='')
            else:
                print(" ", end='')
            print("0x{:02x},".format(b), end='')
        print("\n};")
        print("const unsigned int rsa_pub_key_len = {};".format(len(encoded)))

    def sig_type(self):
        """Return the type of this signature (as a string)"""
        if sign_rsa_pss:
            return "PKCS1_PSS_RSA{}_SHA256".format(self.key_size())
        else:
            return "PKCS15_RSA{}_SHA256".format(self.key_size())

    def sig_len(self):
        return 256 if self.key_size() == 2048 else 384

    def sig_tlv(self):
        return "RSA2048" if self.key_size() == 2048 else "RSA3072"

    def sign(self, payload):
        converted_payload = bytes(payload)
        sha = SHA256.new(converted_payload)
        if sign_rsa_pss:
            signer = PKCS1_PSS.new(self.key)
        else:
            signer = PKCS1_v1_5.new(self.key)
        signature = signer.sign(sha)
        assert len(signature) == self.sig_len()
        return signature

def load(path):
    with open(path, 'rb') as f:
        pem = f.read()
    try:
        key = RSA.importKey(pem)
        return RSAutil(key)
    except ValueError:
        raise Exception("Unsupported RSA key file")
