import sys
sys.path.append('../build/lib.linux-x86_64-2.3')

import sb

class tt_checker(sb.SimModule):

	## Tap States
	e_tlr   = 0;
	e_rti   = 1;
	e_seldr = 2;
	e_selir = 3;
	e_capdr = 4;
	e_capir = 5;
	e_shfdr = 6;
	e_shfir = 7;
	e_extdr = 8;
	e_extir = 9;
	e_pusdr = 10;
	e_pusir = 11;
	e_ex2dr = 12;
	e_ex2ir = 13;
	e_upddr = 14;
	e_updir = 15;
	state_names = ["TestLogicReset", "RunTestIdle", "SelectDR", "SelectIR", "CaptureDR", "CaptureIR", "ShiftDR", "ShiftIR", "ExitDR", "ExitIR", "PushDR", "PushIR", "ExitDR2", "ExitIR2", "UpdateDR", "UpdateIR"];
	nstate_0 = [e_rti, e_rti, e_capdr, e_capir, e_shfdr, e_shfir, e_shfdr, e_shfir, e_pusdr, e_pusir, e_pusdr, e_pusir, e_shfdr, e_shfir, e_rti, e_rti];
	nstate_1 = [e_tlr, e_seldr, e_selir, e_tlr, e_extdr, e_extir, e_extdr, e_extir, e_upddr, e_updir, e_ex2dr, e_ex2ir, e_upddr, e_updir, e_seldr, e_seldr];

	def __init__(self, args):
		self.nstate = self.e_tlr;
		self.cstate = self.e_tlr;
		self.iri = sb.reg();
		self.dri = sb.reg();
		self.iro = sb.reg();
		self.dro = sb.reg();

	def initial(self):
		self.tck = self.wave("SYS.sys_Tck");
		self.cycle = self.wave("ENV.cycle");
		self.tdi = self.wave("SYS.sys_Tdi");
		self.tdo = self.wave("SYS.sys_Tdo");
		self.tms = self.wave("SYS.sys_Tms");
		self.trstl = self.wave("SYS.sys_Trst_L");

	def monitor(self):
#		print "self.cstate:%d self.nstate:%d" % (self.cstate, self.nstate);
		if not self.tck.event or (1 != self.tck.reg()):
			return;
#		print "cycle:%0d tck:%0d tdi:%0d tdo:%0d tms:%0d trstl:%0d" % (self.cycle.i(), self.tck.i(), self.tdi.i(), self.tdo.i(), self.tms.i(), self.trstl.i());

		## Clock edge happens
		self.cstate = self.nstate;
		if 0 == self.tms.reg():
			self.nstate = self.nstate_0[self.cstate];
		else:
			self.nstate = self.nstate_1[self.cstate];

		##
		if self.e_shfir == self.cstate:
			self.iro.push( self.tdo.i(), sb.MSB );
			self.iri.push( self.tdi.i(), sb.MSB );
		if self.e_shfdr == self.cstate:
			self.dro.push( self.tdo.i(), sb.MSB );
			self.dri.push( self.tdi.i(), sb.MSB );

#		if self.nstate != self.cstate:
#			print "cycle:%0d State changed to %s" % (self.cycle, self.state_names[self.cstate]);
		if (self.e_capir == self.cstate) or (self.e_capdr == self.cstate):
			self.iri.resize(0, 0);
			self.dri.resize(0, 0);
			self.iro.resize(0, 0);
			self.dro.resize(0, 0);
			self.iri &= 0;
			self.dri &= 0;
			self.iro &= 0;
			self.dro &= 0;
		if (self.e_updir == self.cstate):
			self.iri.pop();
			self.iro.pop();
			print "time:%0d cycle:%0d Got Update IR IN:%s OUT:%s" % (self.time, self.cycle.i(), self.iri.str(), self.iro.str());
		if (self.e_upddr == self.cstate):
			self.dri.pop();
			self.dro.pop();
			print "time:%0d cycle:%0d Got Update DR IN:%s OUT:%s" % (self.time, self.cycle.i(), self.dri.str(), self.dro.str());

tt_c = tt_checker("sample_fsdb/verilog.fsdb");

tt_c.surf();

