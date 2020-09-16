"""LCM type definitions
This file automatically generated by lcm.
DO NOT MODIFY BY HAND!!!!
"""

try:
    import cStringIO.StringIO as BytesIO
except ImportError:
    from io import BytesIO
import struct

class Voxel_t(object):
    __slots__ = ["id", "pos_x", "pos_y", "pos_z", "occupied"]

    __typenames__ = ["int32_t", "int32_t", "int32_t", "int32_t", "boolean"]

    __dimensions__ = [None, None, None, None, None]

    def __init__(self):
        self.id = 0
        self.pos_x = 0
        self.pos_y = 0
        self.pos_z = 0
        self.occupied = False

    def encode(self):
        buf = BytesIO()
        buf.write(Voxel_t._get_packed_fingerprint())
        self._encode_one(buf)
        return buf.getvalue()

    def _encode_one(self, buf):
        buf.write(struct.pack(">iiiib", self.id, self.pos_x, self.pos_y, self.pos_z, self.occupied))

    def decode(data):
        if hasattr(data, 'read'):
            buf = data
        else:
            buf = BytesIO(data)
        if buf.read(8) != Voxel_t._get_packed_fingerprint():
            raise ValueError("Decode error")
        return Voxel_t._decode_one(buf)
    decode = staticmethod(decode)

    def _decode_one(buf):
        self = Voxel_t()
        self.id, self.pos_x, self.pos_y, self.pos_z = struct.unpack(">iiii", buf.read(16))
        self.occupied = bool(struct.unpack('b', buf.read(1))[0])
        return self
    _decode_one = staticmethod(_decode_one)

    _hash = None
    def _get_hash_recursive(parents):
        if Voxel_t in parents: return 0
        tmphash = (0x305375bcf9de1945) & 0xffffffffffffffff
        tmphash  = (((tmphash<<1)&0xffffffffffffffff) + (tmphash>>63)) & 0xffffffffffffffff
        return tmphash
    _get_hash_recursive = staticmethod(_get_hash_recursive)
    _packed_fingerprint = None

    def _get_packed_fingerprint():
        if Voxel_t._packed_fingerprint is None:
            Voxel_t._packed_fingerprint = struct.pack(">Q", Voxel_t._get_hash_recursive([]))
        return Voxel_t._packed_fingerprint
    _get_packed_fingerprint = staticmethod(_get_packed_fingerprint)
