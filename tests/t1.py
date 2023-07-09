import sys
sys.path.append('../build/lib.linux-x86_64-2.7');

import sb

oc = sb.Ocean("sample_fsdb/verilog.fsdb");
cycle = oc.newWave("ENV.cycle");
clock = oc.newWave("ENV.clock");
tck = oc.newWave("SYS.sys_Tck");
tdi = oc.newWave("SYS.sys_Tdi");
tdo = oc.newWave("SYS.sys_Tdo");
tms = oc.newWave("SYS.sys_Tms");
trstl = oc.newWave("SYS.sys_Trst_L");
#trstl_1 = oc.newWave("SYS.sys_Trst_L_1");

oc.startSurf();

print "trstl has %d indexes..." % trstl.numIdx();
print "cycle has %d indexes..." % cycle.numIdx();
print cycle.Idx(0);
#print trstl.Idx(0);

print "%d" % cycle;
print 1 + cycle + 1 + 1
print cycle;

#while oc.nextVC(tck):
#	if tck != 1:
#		continue;
#	print "cycle:%0d tck:%0d tms:%0d tdi:%0d tdo:%0d trstl:%0d" % (cycle, tck, tms, tdi, tdo, trstl);

#for i in range(1, 10):
#	print i;
#	oc.nextVC(tck);
#	if tck != 1:
#		continue;
#	print "cycle:%0d tck:%0d tms:%0d tdi:%0d tdo:%0d trstl:%0d" % (cycle, tck, tms, tdi, tdo, trstl);

