"""LCM type definitions
This file automatically generated by lcm.
DO NOT MODIFY BY HAND!!!!
"""

try:
    import cStringIO.StringIO as BytesIO
except ImportError:
    from io import BytesIO
import struct

import librav_lcm_msgs.VehicleFootprint

class VehicleState(object):
    __slots__ = ["id", "position", "theta", "speed", "footprint"]

    __typenames__ = ["int64_t", "double", "double", "double", "librav_lcm_msgs.VehicleFootprint"]

    __dimensions__ = [None, [2], None, None, None]

    def __init__(self):
        self.id = 0
        self.position = [ 0.0 for dim0 in range(2) ]
        self.theta = 0.0
        self.speed = 0.0
        self.footprint = librav_lcm_msgs.VehicleFootprint()

    def encode(self):
        buf = BytesIO()
        buf.write(VehicleState._get_packed_fingerprint())
        self._encode_one(buf)
        return buf.getvalue()

    def _encode_one(self, buf):
        buf.write(struct.pack(">q", self.id))
        buf.write(struct.pack('>2d', *self.position[:2]))
        buf.write(struct.pack(">dd", self.theta, self.speed))
        assert self.footprint._get_packed_fingerprint() == librav_lcm_msgs.VehicleFootprint._get_packed_fingerprint()
        self.footprint._encode_one(buf)

    def decode(data):
        if hasattr(data, 'read'):
            buf = data
        else:
            buf = BytesIO(data)
        if buf.read(8) != VehicleState._get_packed_fingerprint():
            raise ValueError("Decode error")
        return VehicleState._decode_one(buf)
    decode = staticmethod(decode)

    def _decode_one(buf):
        self = VehicleState()
        self.id = struct.unpack(">q", buf.read(8))[0]
        self.position = struct.unpack('>2d', buf.read(16))
        self.theta, self.speed = struct.unpack(">dd", buf.read(16))
        self.footprint = librav_lcm_msgs.VehicleFootprint._decode_one(buf)
        return self
    _decode_one = staticmethod(_decode_one)

    _hash = None
    def _get_hash_recursive(parents):
        if VehicleState in parents: return 0
        newparents = parents + [VehicleState]
        tmphash = (0x385f535c024ee9c0+ librav_lcm_msgs.VehicleFootprint._get_hash_recursive(newparents)) & 0xffffffffffffffff
        tmphash  = (((tmphash<<1)&0xffffffffffffffff) + (tmphash>>63)) & 0xffffffffffffffff
        return tmphash
    _get_hash_recursive = staticmethod(_get_hash_recursive)
    _packed_fingerprint = None

    def _get_packed_fingerprint():
        if VehicleState._packed_fingerprint is None:
            VehicleState._packed_fingerprint = struct.pack(">Q", VehicleState._get_hash_recursive([]))
        return VehicleState._packed_fingerprint
    _get_packed_fingerprint = staticmethod(_get_packed_fingerprint)
