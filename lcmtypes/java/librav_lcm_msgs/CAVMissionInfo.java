/* LCM type definition class file
 * This file was automatically generated by lcm-gen
 * DO NOT MODIFY BY HAND!!!!
 */

package librav_lcm_msgs;
 
import java.io.*;
import java.util.*;
import lcm.lcm.*;
 
public final class CAVMissionInfo implements lcm.lcm.LCMEncodable
{
    public long id;
    public int mission_type;
    public librav_lcm_msgs.VehicleState start_state;
    public librav_lcm_msgs.VehicleState goal_state;
 
    public CAVMissionInfo()
    {
    }
 
    public static final long LCM_FINGERPRINT;
    public static final long LCM_FINGERPRINT_BASE = 0xcf592362aa43fd14L;
 
    public static final int MT_NEW_MISSION_REQUEST = 1;
    public static final int MT_ABORT_CURRENT_MISSION = 2;

    static {
        LCM_FINGERPRINT = _hashRecursive(new ArrayList<Class<?>>());
    }
 
    public static long _hashRecursive(ArrayList<Class<?>> classes)
    {
        if (classes.contains(librav_lcm_msgs.CAVMissionInfo.class))
            return 0L;
 
        classes.add(librav_lcm_msgs.CAVMissionInfo.class);
        long hash = LCM_FINGERPRINT_BASE
             + librav_lcm_msgs.VehicleState._hashRecursive(classes)
             + librav_lcm_msgs.VehicleState._hashRecursive(classes)
            ;
        classes.remove(classes.size() - 1);
        return (hash<<1) + ((hash>>63)&1);
    }
 
    public void encode(DataOutput outs) throws IOException
    {
        outs.writeLong(LCM_FINGERPRINT);
        _encodeRecursive(outs);
    }
 
    public void _encodeRecursive(DataOutput outs) throws IOException
    {
        outs.writeLong(this.id); 
 
        outs.writeInt(this.mission_type); 
 
        this.start_state._encodeRecursive(outs); 
 
        this.goal_state._encodeRecursive(outs); 
 
    }
 
    public CAVMissionInfo(byte[] data) throws IOException
    {
        this(new LCMDataInputStream(data));
    }
 
    public CAVMissionInfo(DataInput ins) throws IOException
    {
        if (ins.readLong() != LCM_FINGERPRINT)
            throw new IOException("LCM Decode error: bad fingerprint");
 
        _decodeRecursive(ins);
    }
 
    public static librav_lcm_msgs.CAVMissionInfo _decodeRecursiveFactory(DataInput ins) throws IOException
    {
        librav_lcm_msgs.CAVMissionInfo o = new librav_lcm_msgs.CAVMissionInfo();
        o._decodeRecursive(ins);
        return o;
    }
 
    public void _decodeRecursive(DataInput ins) throws IOException
    {
        this.id = ins.readLong();
 
        this.mission_type = ins.readInt();
 
        this.start_state = librav_lcm_msgs.VehicleState._decodeRecursiveFactory(ins);
 
        this.goal_state = librav_lcm_msgs.VehicleState._decodeRecursiveFactory(ins);
 
    }
 
    public librav_lcm_msgs.CAVMissionInfo copy()
    {
        librav_lcm_msgs.CAVMissionInfo outobj = new librav_lcm_msgs.CAVMissionInfo();
        outobj.id = this.id;
 
        outobj.mission_type = this.mission_type;
 
        outobj.start_state = this.start_state.copy();
 
        outobj.goal_state = this.goal_state.copy();
 
        return outobj;
    }
 
}
