import sys
sys.path.append('../build/lib.linux-x86_64-2.3')

from sb import *

oc = Ocean("sample_fsdb/verilog.fsdb");

cycle = oc.newWave("ENV.cycle");
scope = oc.newWave("ENV.dump_scope");
cap_tdi = oc.newWave("SYS.v_tap_check.cap_tdi");
ctrl_st = oc.newWave("SYS.v_jtag_drv.v_jtag_drv.Next_Drv_CtrlState");

oc.startSurf();

print cycle;
oc.nextVC(cycle);
oc.nextVC(cycle);
oc.nextVC(cycle);

print "cycle:%x" % int(cycle);
print "cap_tdi:%x" % int(cap_tdi);
print "ctrl_st:%x" % int(ctrl_st);
print scope;
tt = scope + 1;
print type(tt);
print "scope:", int(scope);
isc = int(scope);
print "isc 1:", isc;
cr = reg(31,0);
print "cr 1:", cr;
cr &= isc;
print "cr 2:", cr;
print "isc 2:", isc;

print type(scope.reg());
ii = scope.i(DATA);
print "%d" % ii;
print "%o" % ii;
print "%s" % oct(scope);
##print scope.reg().__str__;

print "#########################################"
print "%s" % scope.str(BIN);

cr2 = reg(31,0);
cr2 += 20;
print type(cr2);
print cr2;
cr3 = cr2.push(3, MSB);
print cr2;
print cr3.str(BIN);
