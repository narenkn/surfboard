import sys
sys.path.append('../build/lib.linux-x86_64-2.7')

from sb import *

r2 = reg();
r1 = reg();
##r2 = reg("40'hx");
##r1 = reg(31, 0, "32'hx");
print type(r1);
##r1 *= 3;
print type(r1);
if r1:
	print "r1 is true";
else:
	print "r1 is false";
if r2:
	print "r2 is true";
else:
	print "r2 is false";
print r1;
print r2;

oc = Ocean("sample_fsdb/verilog.fsdb");
clock = oc.newWave("ENV.clock");

print type(clock);
print clock;

cycle = oc.newWave("ENV.cycle")
tck = oc.newWave("SYS.sys_Tck")
tdi = oc.newWave("SYS.sys_Tdi")
tdo = oc.newWave("SYS.sys_Tdo")
tms = oc.newWave("SYS.sys_Tms")
trstl = oc.newWave("SYS.sys_Trst_L")
#trstl_1 = oc.newWave("SYS.sys_Trst_L_1", 5);

oc.startSurf();

print "trstl has %d indexes..." % trstl.numIdx();
print "cycle has %d indexes..." % cycle.numIdx();
print cycle.Idx(0);
#print trstl.Idx(0);

print "%d" % int(cycle);
print "hehe";
print type(cycle);
print 1 + cycle + 1 + 1
print "hehe 2";
print cycle;

for i in range(1, 10):
	print i;
	oc.nextVC(tck);
	if tck == 0:
		print "tck continue for %0d" % tck;
		continue;
	print "cycle:%0d tck:%0d tms:%0d tdi:%0d tdo:%0d trstl:%0d" % (int(cycle), tck, tms, tdi, tdo, trstl);
#	print "cycle:%0d tck:%0d tms:%0d tdi:%0d tdo:%0d trstl:%0d trstl:%0d trstl:%0d" % (cycle[1], tck[0], tms[-1], tdi[-2], tdo[-3], trstl[-4], trstl[-5], trstl[-6]);
