# UDP server example
import socket
import xml.parsers.expat
import re
import os
import signal
from threading import Thread

all_receives = {};
all_owner_info = {};

def getset_owner_info():
	global all_owner_info;
	for line in os.popen('getent passwd', 'r'):
		match = line.split(r':');
		all_owner_info[match[2]] = (match[0], match[4]);

class MyParser:
	re_dotslash = re.compile(r'[.\/]');
	# prepare for parsing
	def __init__(self):
		self.Parser = xml.parsers.expat.ParserCreate();
		self.Parser.CharacterDataHandler = self.handleCharData;
		self.Parser.StartElementHandler = self.handleStartElement;
		self.Parser.EndElementHandler = self.handleEndElement;
		self.tag = None;
		self.Parser.Parse("<root>", False);

	def __del__(self):
		self.Parser.Parse("</root>", True);

	def check_format_info(self, info):
		global all_receives, all_owner_info;
		if "debug" in info:
			return;
		assert("user" in info);
		assert("owner" in info);
		assert("date" in info);
		assert(info["owner"] in all_owner_info);
		## Create date format
		spl = info["date"].split(r':');
		if '0' == spl[1][0]:
			spl[1] = spl[1][1];
		print info["date"];
		print spl;
		info["clsdate"] = "%s%s%s" % (spl[0], spl[1], spl[2]);
		## Format script path
		tmp = info["program"];
		tmp = self.re_dotslash.sub("_", tmp);
		info["clsprogram"] = tmp;
		## Finally save
		all_receives[info["user"]] = info;

	# parse the XML file
	def parse(self, xml_string):
		self.obj = {};
		try:
			self.Parser.Parse(xml_string, False);
		except:
			print "ERROR: While parsing XML!";
			return None;
		self.check_format_info(self.obj);
		return self.obj;

	# 3 handler functions
	def handleStartElement(self, name, attrs): self.tag = name;
	def handleEndElement(self, name): pass
	def handleCharData(self, data):
		if self.tag not in self.obj:
			self.obj[self.tag] = data;

def listen_update():
	server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	server_socket.bind(("", 9600))

	print"UDPServer Waiting for client on port 9600"
	p = MyParser();

	while 1:
		data, address = server_socket.recvfrom(256)
		obj = p.parse(data);
		## Not a debug communication
		if "debug" in obj:
			print "Obtained debug communication : ", obj;

htm_header = """
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html lang="en">
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
	<title>Script Logging Utility</title>
		<style type="text/css">
			table {
				color: #7F7F7F;
				font: 1.0em/2.0em "Trebuchet MS",Verdana,sans-serif;
				border-collapse: collapse
			}
			table,caption {
				margin: 0 auto;
				border-right: 1px solid #CCC;
				border-left: 1px solid #CCC
			}
			caption,th,td {
				border-left: 0;
				padding: 10px
			}
			caption,thead th,tfoot th,tfoot td {
				background-color: #E63C1E;
				color: #FFF;
				font-weight: bold;
				text-transform: uppercase
			}
			thead th {
				background-color: #C30;
				color: #FFB3A6;
				text-align: center
			}
			tbody th {
				padding: 20px 10px
			}
			tbody tr.odd {
				background-color: #F7F7F7;
				color: #666
			}
			tbody a {
				padding: 1px 2px;
				color: #333;
				text-decoration: none;
				border-bottom: 1px dotted #E63C1E
			}
			tbody a:active,tbody a:hover,tbody a:focus,tbody a:visited {
				color: #666
			}
			tbody tr:hover {
				background-color: #EEE;
				color: #333
			}
			tbody tr:hover a {
				background-color: #FFF
			}
			tbody td+td+td+td a {
				color: #C30;
				font-weight: bold;
				border-bottom: 0
			}
			tbody td+td+td+td a:active,tbody td+td+td+td a:hover,tbody td+td+td+td a:focus,tbody td+td+td+td a:visited {
				color: #E63C1E
			}
			tbody td a[href="http://www.rodcast.com.br/"] {
				margin: 0 auto;
				display: block;
				width: 15px;
				height: 15px;
				background: transparent url('data:image/gif;base64,R0lGODlhDwAPAIAAACEpMf///yH5BAAAAAAALAAAAAAPAA8AAAIjjA8Qer0JmYvULUOlxXEjaEndliUeA56c97TqSD5pfJnhNxYAOw%3D%3D') no-repeat;
				text-indent: -999em;
				border-bottom: 0
			}
			tbody a:visited:after {
				font-family: Verdana,sans-serif;
				content: "\00A0\221A"
			}
			body {
				font: .8em "Lucida Grande", Tahoma, Arial, Helvetica, sans-serif;
			}
			ol {
				margin:0;
				padding: 0 1.5em;
			}
			.hidden {
				display:none;
			}
		</style>
		<script type="text/javascript">
			// Title: Timestamp picker
			// Description: See the demo at url
			// URL: http://us.geocities.com/tspicker/
			// Script featured on: http://javascriptkit.com/script/script2/timestamp.shtml
			// Version: 1.0
			// Date: 12-05-2001 (mm-dd-yyyy)
			// Author: Denis Gritcyuk <denis@softcomplex.com>; <tspicker@yahoo.com>
			// Notes: Permission given to use this script in any kind of applications if
			//    header lines are left unchanged. Feel free to contact the author
			//    for feature requests and/or donations

			function show_calendar(str_target, str_datetime) {
				var arr_months = ["January", "February", "March", "April", "May", "June",
					"July", "August", "September", "October", "November", "December"];
				var week_days = ["Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"];
				var n_weekstart = 1; // day week starts from (normally 0 or 1)

				var dt_datetime = (str_datetime == null || str_datetime =="" ?  new Date() : str2dt(str_datetime));
				var dt_prev_month = new Date(dt_datetime);
				dt_prev_month.setMonth(dt_datetime.getMonth()-1);
				var dt_next_month = new Date(dt_datetime);
				dt_next_month.setMonth(dt_datetime.getMonth()+1);
				var dt_firstday = new Date(dt_datetime);
				dt_firstday.setDate(1);
				dt_firstday.setDate(1-(7+dt_firstday.getDay()-n_weekstart)%7);
				var dt_lastday = new Date(dt_next_month);
				dt_lastday.setDate(0);

				// html generation (feel free to tune it for your particular application)
				// print calendar header
				var str_buffer = new String (
					"<html>\n"+
					"<head>\n"+
					"	<title>Calendar</title>\n"+
					"</head>\n"+
					"<body bgcolor=\"White\">\n"+
					"<table class=\"clsOTable\" cellspacing=\"0\" border=\"0\" width=\"100%\">\n"+
					"<tr><td bgcolor=\"#4682B4\">\n"+
					"<table cellspacing=\"1\" cellpadding=\"3\" border=\"0\" width=\"100%\">\n"+
					"<tr>\n	<td bgcolor=\"#4682B4\"><a href=\"javascript:window.opener.show_calendar('"+
					str_target+"', '"+ dt2dtstr(dt_prev_month)+"'+document.cal.time.value);\">"+
					"<img src=\"prev.gif\" width=\"16\" height=\"16\" border=\"0\""+
					" alt=\"previous month\"></a></td>\n"+
					"	<td bgcolor=\"#4682B4\" colspan=\"5\">"+
					"<font color=\"white\" face=\"tahoma, verdana\" size=\"2\">"
					+arr_months[dt_datetime.getMonth()]+" "+dt_datetime.getFullYear()+"</font></td>\n"+
					"	<td bgcolor=\"#4682B4\" align=\"right\"><a href=\"javascript:window.opener.show_calendar('"
					+str_target+"', '"+dt2dtstr(dt_next_month)+"'+document.cal.time.value);\">"+
					"<img src=\"next.gif\" width=\"16\" height=\"16\" border=\"0\""+
					" alt=\"next month\"></a></td>\n</tr>\n"
				);

				var dt_current_day = new Date(dt_firstday);
				// print weekdays titles
				str_buffer += "<tr>\n";
				for (var n=0; n<7; n++)
					str_buffer += "	<td bgcolor=\"#87CEFA\">"+
					"<font color=\"white\" face=\"tahoma, verdana\" size=\"2\">"+
					week_days[(n_weekstart+n)%7]+"</font></td>\n";
				// print calendar table
				str_buffer += "</tr>\n";
				while (dt_current_day.getMonth() == dt_datetime.getMonth() ||
					dt_current_day.getMonth() == dt_firstday.getMonth()) {
					// print row heder
					str_buffer += "<tr>\n";
					for (var n_current_wday=0; n_current_wday<7; n_current_wday++) {
							if (dt_current_day.getDate() == dt_datetime.getDate() &&
								dt_current_day.getMonth() == dt_datetime.getMonth())
								// print current date
								str_buffer += "	<td bgcolor=\"#FFB6C1\" align=\"right\">";
							else if (dt_current_day.getDay() == 0 || dt_current_day.getDay() == 6)
								// weekend days
								str_buffer += "	<td bgcolor=\"#DBEAF5\" align=\"right\">";
							else
								// print working days of current month
								str_buffer += "	<td bgcolor=\"white\" align=\"right\">";

							if (dt_current_day.getMonth() == dt_datetime.getMonth())
								// print days of current month
								str_buffer += "<a href=\"javascript:window.opener."+str_target+
								".value='"+dt2dtstr(dt_current_day)+"'+document.cal.time.value; window.close();\">"+
								"<font color=\"black\" face=\"tahoma, verdana\" size=\"2\">";
							else 
								// print days of other months
								str_buffer += "<a href=\"javascript:window.opener."+str_target+
								".value='"+dt2dtstr(dt_current_day)+"'+document.cal.time.value; window.close();\">"+
								"<font color=\"gray\" face=\"tahoma, verdana\" size=\"2\">";
							str_buffer += dt_current_day.getDate()+"</font></a></td>\n";
							dt_current_day.setDate(dt_current_day.getDate()+1);
					}
					// print row footer
					str_buffer += "</tr>\n";
				}
				// print calendar footer
				str_buffer +=
					"<form name=\"cal\">\n<tr><td colspan=\"7\" bgcolor=\"#87CEFA\">"+
					"<font color=\"White\" face=\"tahoma, verdana\" size=\"2\">"+
					"Time: <input type=\"text\" name=\"time\" value=\""+dt2tmstr(dt_datetime)+
					"\" size=\"8\" maxlength=\"8\"></font></td></tr>\n</form>\n" +
					"</table>\n" +
					"</tr>\n</td>\n</table>\n" +
					"</body>\n" +
					"</html>\n";

				var vWinCal = window.open("", "Calendar", 
					"width=200,height=250,status=no,resizable=yes,top=200,left=200");
				vWinCal.opener = self;
				var calc_doc = vWinCal.document;
				calc_doc.write (str_buffer);
				calc_doc.close();
			}

			// datetime parsing and formatting routimes. modify them if you wish other datetime format
			function str2day (str_datetime) {
				var re_date = /^(\d+)\-(\d+)\-(\d+)\s+(\d+)\:(\d+)\:(\d+)$/;
				if (!re_date.exec(str_datetime))
					return alert("Invalid Datetime format: "+ str_datetime);
				return (new String (RegExp.$3, RegExp.$2-1, RegExp.$1));
			}
			function str2dt (str_datetime) {
				var re_date = /^(\d+)\-(\d+)\-(\d+)\s+(\d+)\:(\d+)\:(\d+)$/;
				if (!re_date.exec(str_datetime))
					return alert("Invalid Datetime format: "+ str_datetime);
				return (new Date (RegExp.$3, RegExp.$2-1, RegExp.$1, RegExp.$4, RegExp.$5, RegExp.$6));
			}
			function dt2dtstr (dt_datetime) {
				return (new String (dt_datetime.getDate()+"-"+(dt_datetime.getMonth()+1)+"-"+dt_datetime.getFullYear()+" "));
			}
			function dt2repr (dt_datetime) {
				return (new String (dt_datetime.getFullYear()+""+(dt_datetime.getMonth()+1)+""+dt_datetime.getDate()));
			}
			function dt2tmstr (dt_datetime) {
				return (new String (
				dt_datetime.getHours()+":"+dt_datetime.getMinutes()+":"+dt_datetime.getSeconds()));
			}
			function hasClass(ele,cls) {
				return ele.className.match(new RegExp('(\\s|^)'+cls+'(\\s|$)'));
			}
			function addClass(ele,cls) {
				if (!hasClass(ele,cls)) ele.className += " "+cls;
			}
			function removeClass(ele,cls) {
				if (hasClass(ele,cls)) {
					var reg = new RegExp('(\\s|^)'+cls+'(\\s|$)');
					ele.className=ele.className.replace(reg,' ');
				}
			}
			function toggleClass(ele,cls) {
				if (hasClass(ele,cls)) {
					var reg = new RegExp('(\\s|^)'+cls+'(\\s|$)');
					ele.className=ele.className.replace(reg,' ');
				} else {
					ele.className += " "+cls;
				}
			}
			function getElementsByClass(searchClass,node,tag) {
				var classElements = new Array();
				if ( node == null )
					node = document;
				if ( tag == null )
					tag = '*';
				var els = node.getElementsByTagName(tag);
				var elsLen = els.length;
				var pattern = new RegExp("(^|\\s)"+searchClass+"(\\s|$)");
				for (i = 0, j = 0; i < elsLen; i++) {
					if ( pattern.test(els[i].className) ) {
						classElements[j] = els[i];
						j++;
					}
				}
				return classElements;
			}
			function removeClassElements(col) {
				var els = getElementsByClass(col,document.func_table,null);
				for (i = 0, j = 0; i < els.length; i++) {
					addClass(els[i], 'hidden');
				}
			}
			function addClassElements(col) {
				var els = getElementsByClass(col,document.func_table,null);
				for (i = 0, j = 0; i < els.length; i++) {
					removeClass(els[i], 'hidden');
				}
			}
			function hideallRows() {
				var tables = document.getElementsByTagName("table");
				for (var i=0; i<tables.length; i++) {
					var rows = tables[i].getElementsByTagName("tr");
					for (var j=1; j<rows.length; j++) {
						addClass(rows[j],"hidden");
					}
				}
			}
			function unhideallRows() {
				var tables = document.getElementsByTagName("table");
				for (var i=0; i<tables.length; i++) {
					var rows = tables[i].getElementsByTagName("tr");
					for (var j=1; j<rows.length; j++) {
						removeClass(rows[j],"hidden");
					}
				}
			}
			function ApplyORConstraint() {
				/* start afresh */
				hideallRows();
				/* user constraint */
				if ("" != document.Constraint.user.value) {
					addClassElements(document.Constraint.user.value);
				}
				/* owner constraint */
				if ("" != document.Constraint.owner.value) {
					addClassElements(document.Constraint.owner.value);
				}
				/* script constraint */
				if ("" != document.Constraint.script.value) {
					addClassElements(document.Constraint.script.value);
				}
				/* Date constraint */
				if ("" == document.Constraint.st_time.value) {
					st_date = str2dt("1-1-2011 00:00:00");
				} else {
					st_date = str2dt(document.Constraint.st_time.value);
				}
				if ("" == document.Constraint.en_time.value) {
					en_date = new Date();
				} else {
					en_date = str2dt(document.Constraint.en_time.value);
				}
//				alert("Start Date: " + dt2dtstr(st_date) + " " + dt2repr(st_date));
//				alert("End Date: " + dt2dtstr(en_date));
				/* Loop st_date till en_date */
				while ( (st_date.getYear() <= en_date.getYear()) &&
					( ( (st_date.getMonth() == en_date.getMonth()) && (st_date.getDate() <= en_date.getDate()) ) ||
					(st_date.getMonth() < en_date.getMonth()) )
					) {
//					alert("Looping through" + dt2dtstr(st_date));
//					alert("Date repr '" + dt2repr(st_date) + "'");
					var els = getElementsByClass(dt2repr(st_date),document,null);
					for (i = 0; i < els.length; i++) {
						removeClass(els[i], 'hidden');
					}
					st_date.setDate(st_date.getDate()+1);
				}
			}
			function ApplyConstraint() {
				/* start afresh */
				hideallRows();
				/* user constraint */
				if ("" != document.Constraint.user.value) {
					addClassElements(document.Constraint.user.value);
				}
				/* owner constraint */
				if ("" != document.Constraint.owner.value) {
					addClassElements(document.Constraint.owner.value);
				}
				/* script constraint */
				if ("" != document.Constraint.script.value) {
					addClassElements(document.Constraint.script.value);
				}
				/* Date constraint */
				if ("" == document.Constraint.st_time.value) {
					st_date = str2dt("1-1-2011 00:00:00");
				} else {
					st_date = str2dt(document.Constraint.st_time.value);
				}
				if ("" == document.Constraint.en_time.value) {
					en_date = new Date();
				} else {
					en_date = str2dt(document.Constraint.en_time.value);
				}
//				alert("Start Date: " + dt2dtstr(st_date) + " " + dt2repr(st_date));
//				alert("End Date: " + dt2dtstr(en_date));
				/* Loop st_date till en_date */
				while ( (st_date.getYear() <= en_date.getYear()) &&
					( ( (st_date.getMonth() == en_date.getMonth()) && (st_date.getDate() <= en_date.getDate()) ) ||
					(st_date.getMonth() < en_date.getMonth()) )
					) {
//					alert("Looping through" + dt2dtstr(st_date));
//					alert("Date repr '" + dt2repr(st_date) + "'");
					var els = getElementsByClass(dt2repr(st_date),document,null);
					for (i = 0; i < els.length; i++) {
						removeClass(els[i], 'hidden');
					}
					st_date.setDate(st_date.getDate()+1);
				}
			}
		</script>
	</head>
	<body>
			<h1>PyDisplay File script log utility</h1>
			<form name="Constraint"> <P>
				<h2>Filter Controls</h2>
				From Date:<input type="Text" name="st_time" value=""> <a href="javascript:show_calendar('document.Constraint.st_time', document.Constraint.st_time.value);"><img src="http://www.mricons.com/store/png/2260_3307_64_cal_icon.png" width="16" height="16" border="0" alt="Click Here to Pick up the st_time"></a>
				To Date:<input type="Text" name="en_time" value=""> <a href="javascript:show_calendar('document.Constraint.en_time', document.Constraint.en_time.value);"><img src="http://www.mricons.com/store/png/2260_3307_64_cal_icon.png" width="16" height="16" border="0" alt="Click Here to Pick up the en_time"></a>
				<BR>
				<BR>
				User ID:<input type="Text" name="user">
				Owner ID:<input type="Text" name="owner">
				<BR>
				<BR>
				Script Name:<input type="Text" name="script">
				<BR>
				<BR>
				<input type="button" value="OR-Table" onclick="ApplyORConstraint();" />
				<input type="button" value="AND-Table" onclick="ApplyConstraint();" />
			</P> </form>
			<BR>
			<BR>
			<BR>
			<BR>
			<table name='func_table'>
				<caption>Script Logs</caption>
				<col id="user"> <col id="name"> <col id="script"> <col id="scriptowner"> <col id="datentime">
			<thead>
				<tr>
					<th class="user">User</th>
					<th class="name">Name</th>
					<th class="script">Script</th>
					<th class="scriptowner">Script Owner</th>
					<th class="datentime">Run Date/Time</th>
				</tr>
			</thead>
			<tbody>
""";
htm_footer = """
			</tbody>
		</table>
	</body>
</html>

""";

def info2tr(info):
	return """<tr class="%s" class="%s" class="%s"><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>""" % (
			info["clsdate"],
			info["clsprogram"],
			info["user"],
			info["user"],
			"",
			info["program"],
			all_owner_info[info["owner"]][0],
			info["date"]
		);

def save_me(signum, frame):
	print 'Signal handler called with signal : ', signum;
	for r in all_receives:
		print info2tr(all_receives[r]);

if "__main__" == __name__:

	getset_owner_info();
	t = Thread(target=listen_update);
	t.start();

	signal.signal(signal.SIGALRM, save_me);
	while (t.isAlive()):
		# Set the signal handler and a 5-second alarm
		signal.alarm(2);
		signal.pause();

	t.join();

