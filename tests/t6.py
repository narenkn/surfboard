import sys
sys.path.append('../build/lib.linux-x86_64-2.3')

from sb import *

class tt_checker(SimModule):

	def __init__(self, args):
		print "__init__ of tt_checker called : hurray";
#		super(Checker, self).__init__(args);
		pass;

	def options(self):
		print "Options of tt_checker called : hurray";

	def early_initial(self):
		print "Early-Initial of tt_checker called : hurray";

	def initial(self):
		print "Initial of tt_checker called : hurray";
#		self.add_callback("ENV.cycle");
		w1 = self.wave("ENV.cycle");

	def monitor(self):
		print "Monitor of tt_checker called : hurray";

	def sequential(self):
		print "Sequential of tt_checker called : hurray";

	def final(self):
		print "Final of tt_checker called : hurray";


tt_c = tt_checker("sample_fsdb/verilog.fsdb");

tt_c.surf();

