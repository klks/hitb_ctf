"""
Released under the MIT-license:

Copyright (c) 2010 Earl Marcus

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
"""
import sys, pkg_resources, imp
import os
import hashlib
import ssh
from time import localtime
from time import sleep
import socket

try:
	import MySQLdb
except:
	print "Please make sure you have MySQLdb working"
	raise
	#sys.exit(1)
	
#Debug mode
debug_mode = True

#Return Values
CTF_SUCCESS	=	0
CTF_FAIL	=	-1
CTF_INTERR	=	-2

team = {}
checksums = {}
daemon_path = {}
flags = {}
ports = []
exit_value = CTF_INTERR

#KEY:(CheckIfExists,Variable)
ctf_config = {
 			"SSH_PORT":[0,None],
			"SSH_UNAME":[0,None],
			"SSH_PUBKEY":[1,None],
			"SSH_PRIVKEY":[1,None],
			
			"DB_UNAME":[0,None],
			"DB_PASSWD":[0,None],
			"DB_HOST":[0,None],
			"DB_SCHEMA":[0,None],
			}

#Db connection and cursor holder
db_conn = None
db_cursor = None

CTF_GETFLAG			=	1
CTF_SETFLAG			=	2
CTF_CHECKFLAG		=	3
CTF_GETHASHFILE		=	4
CTF_CHECKHASHFILE	=	5


def usage():
	global exit_value
	
	print("Usage: god.py [options] <param.1> <param.2> <param.n>")
	print("[Options]");
	print("\t-ec\t\t\t\tExamine configs for errors\n")
	
	#Checks
	print("\t-cf <teamIP> <daemonPort>\tChecks for flag problems")
	print("\t-cx <teamIP> <daemonPort>\tChecks file hash of daemon")

	#Gets
	print("\t-gf <teamIP> <daemonPort>\tGets the flag for a particular teams daemon")
	print("\t-gx <teamIP> <daemonPort>\tGet file hash of daemon")
	
	#Sets
	print("\t-sf <teamIP> <daemonPort>\tSets the flag for a particular teams daemon")

	print("\n[Notes]")
	print("\t<teamIP>\t\t\tSet to 0 to enumerate all IP")
	print("\t<daemonPort>\t\t\tSet to 0 to enumerate all ports")
	
	Log("Exit code : %s" % (CTF_INTERR,), True)
	Log("-------END-------\n", True)
	sys.exit(CTF_INTERR)

#Debug logging
def DebugLog(msg):
	global debug_mode
	
	if debug_mode:
		print "!!!DEBUG :",
		print msg
		
#General purpose logging	
def Log(msg, suppress_print=False):
	log = open("god.log", 'a')
	log.write("[%.2d:%.2d] " % localtime()[3:5])
	log.write(msg + "\n")
	log.close()
	if not suppress_print:
		print msg
	
#Checks if a file exists
def file_exists(filename):
	if not os.path.exists(filename):
		print "Make sure file %s exists" % filename
		usage()

#Checks if a directory exists
def dir_exists(dire):
	if not os.path.isdir(dire):
		print "Make sure directory %s exists" % dire
		usage()

#Opens and sha256 a fie
def sha256file(filename):
	if not os.path.exists(filename):
		return "0"
		
	f = open(filename)
	sha = hashlib.sha256()
	while True:
		data = f.read(1024)
		if not data:
			break
		sha.update(data)
	f.close()
	
	return sha.hexdigest()
						
def defaultConnCheck(ip, port):
	DebugLog("Trying to connect to %s on port %s" % (ip, port,))
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	try:
		s.connect((ip,port))
		s.close()
		return CTF_SUCCESS
	except:
		return CTF_FAIL
		
def ExamineConfigs():
	global team, checksums, daemon_path, flags, ports
	global ctf_config

	#General configs
	
	#Check if anything is missing
	for key in ctf_config:
		if ctf_config[key][1] == None:
			Log("%s variable missing" % (key,))
			
	if ports == []:
		Log("PORTS varible missing, please fix and re-run checks")
		return
	
	for p in ports:
		#Make sure all ports have a valid checksum
		if p not in checksums:
			Log("No checksum for port %s" % p)
		#Make sure all ports have a valid daemon path
		if p not in daemon_path:
			Log("No daemon_path for port %s" % p)
	
	#Make sure all teams have a valid set of ports & flag
	for t in team:
		DebugLog(t)
		for p in ports:
			DebugLog(p)
			if not flags.has_key(t) or p not in flags[t]:
				Log("Team %s(%s) does not have flag configured for port %s" % (team[t][0],t,p))
	
def dbQuery(c,q):
	try:
		DebugLog("Executing : %s" % q)
		c.execute(q)
		return c
	except:
		return None
		
def LoadTeamData():
	global db_cursor
	global team, checksums, daemon_path, ports

	#Query db for ports
	if dbQuery(db_cursor, "select port from ports") == None:
		Log("!!!Unable to get PORTS data")
		usage()
	rows = db_cursor.fetchall()
	
	for row in rows:
		DebugLog("Port : %s" % (row[0],))
		if row[0] in ports:
			Log("Duplicate port in db : %s", (row[0],))
		else:
			ports.append(row[0])
	
	#                     0         1        2
	#keyInfo = {"TAG":(SQLQuery,SQLErrMsg,dictKey)
	keyInfo = {"TINFO":("select ip_address, team_name from team_info", "!!!Unable to get TEAM_INFO data", team),
				"CSUM":("select port,checksum from daemon_checksum", "!!!Unable to get DAEMON_CHECKSUM data", checksums),
				"DPATH":("select port, dpath, dbin, flag_file, valid_response_script from daemon_info", "!!!Unable to get DAEMON_INFO data", daemon_path),
	}
			
	for key in keyInfo:
		if dbQuery(db_cursor, keyInfo[key][0]) == None:
			Log(keyInfo[key][1])
			usage()
		rows = db_cursor.fetchall()
		
		for row in rows:
			#Debug msgs
			if key == "TINFO":
				DebugLog("IP : %s Team : %s" % (row[0],row[1],))
			elif key == "CSUM":
				DebugLog("Port : %s Checksum : %s" % (row[0], row[1],))
			elif key == "DPATH":
				DebugLog("Port : %s DPath : %s DBin : %s FlagFile : %s VRS: %s" % (row[0], row[1], row[2], row[3], row[4],))
				
			#Store data
			if row[0] in keyInfo[key][2]:
				Log("Duplicate entry in db for key : %s" % (key))
			else:
				keyInfo[key][2][row[0]] = row[1:]
	
	#Query db for flags
	if dbQuery(db_cursor, "select ip_address, port, flag, flag_hash from active_flags") == None:
		Log("!!!Unable to get ACTIVE_FLAGS data")
		usage()
	rows = db_cursor.fetchall()
	
	for row in rows:
		DebugLog("IP : %s Port : %s Flag : %s FlagHash : %s" % (row[0],row[1],row[2],row[3],))
		
		ip = row[0]
		port = row[1]
		flag = row[2]
		flaghash = row[3]
		
		#Check if this is a legit IP
		if ip not in team:
			Log("Invalid IP in flag table: %s" % (ip,))
			usage()
			
		#Check for duplicate ip and port match
		if ip in flags and port in flags[ip]:
			Log("Duplicate IP/Port entry : %s:%s" % (ip,port,))
			usage()
				
		#Add this entry into flags
		if ip not in flags:
			flags[ip] = {port:(flag,flaghash)}
		else:
			flags[ip] = dict(flags[ip].items() + [(port,(flag,flaghash))])
	
def ParseConfig():
	global ctf_config
	
	file_exists("ctf.config")
	
	f = open("ctf.config")
	data = f.readlines()
	f.close()
			
	for line in data:
		line = line.strip("\n")
		
		#Skip comments
		if line.startswith("#") or line.startswith(";"): continue
		#Skip empty lines
		if len(line) == 0: continue
		
		#Check if there is an assignment "="
		if line.find("=") == -1:
			print "Malformed line : %s" % line
			usage()
			
		#Split line
		key, value = line.split("=")
		
		#Parse single key data
		if key in ctf_config:
			#print key
			ctf_config[key][1] = value.strip("\r\n")
			#print ctf_config[key][1]
			if ctf_config[key][0] == 1:
				file_exists(ctf_config[key][1])

		else:
			print "Unknown config : %s" % line
			usage()

def EnumCleanup(myssh):
	#Remove dl_tml
	if myssh != None:
		myssh.execute("rm -f dl_tmp")
	if os.path.exists("dl_tmp"):
		os.remove("dl_tmp")
	
def getFile(myssh, file):
	global ctf_config
	
	#Remove temp file if exists
	if os.path.exists("dl_tmp"):
		os.remove("dl_tmp")
		
	#Try making a copy
	myssh.execute("sudo rm -f ./dl_tmp")	#Just incase a file already exists
	cmd_exec = "sudo cp %s ./dl_tmp" % (file,)
	myssh.execute(cmd_exec)
	
	#NOTE: By default file is given permission 750
	
	#Correct permissions
	myssh.execute("sudo chown %s ./dl_tmp" % (ctf_config["SSH_UNAME"][1],))
	
	#Now try downloading file
	try:
		myssh.get("dl_tmp", "dl_tmp")
	except:
		Log("Unable to download file %s" % (file,))
		return CTF_INTERR
		
	return CTF_SUCCESS
	
def getFileHash(myssh, file):
	DebugLog("Hashing file : %s" % (file,))
	ret = getFile(myssh, file)
	if ret != CTF_SUCCESS:
		return CTF_INTERR
		
	#Hash dl_tmp file
	file_hash = sha256file("dl_tmp")		

	#Cleanup
	myssh.execute("rm -f dl_tmp")
	if os.path.exists("dl_tmp"):
		os.remove("dl_tmp")
	
	return file_hash	
		
def EnumClients2(ip, port, op):
	global daemon_path, flags
	global team, ports, exit_value
	global ctf_config
	
	DebugLog("EnumClients2 : %s %s %s" % (ip,port,op))
	
	#SSH to the target
	try:
		myssh = ssh.Connection(ip, ctf_config["SSH_UNAME"][1], ctf_config["SSH_PRIVKEY"][1], port=int(ctf_config["SSH_PORT"][1]))
	except:
		Log("!!! %s(%s:%s) Unable to establish an SSH connection" % (team[ip][0], ip, ctf_config["SSH_PORT"][1]))
		exit_value = CTF_FAIL
		return
	
	#Log nothing
	myssh.execute("HISTFILE=/dev/null")
	
	#The reason port is enumerated here is because we only need 1 ssh connection and not to
	#establish a new connection for every port which slows shit down a ton.
	for p in ports:
		if port == 0 or p == port:
			#Check if daemon_path has this port
			if p not in daemon_path:
				Log("Port %s does not exist in daemon_info for %s" % (p,ip))
				exit_value = CTF_INTERR
				continue

			#Check if we have a valid flag entry for this IP and port
			if ip not in flags or p not in flags[ip]:
				Log("Unable to find flag data for %s:%s" % (ip,p))
				exit_value = CTF_INTERR
				continue
		
			if op == CTF_GETFLAG or op == CTF_CHECKFLAG:
				#Check if flag file is legit
				user_hash = getFileHash(myssh, daemon_path[p][0] + daemon_path[p][2])
				if user_hash == CTF_INTERR:
					EnumCleanup(myssh)
					continue
				
				if op == CTF_GETFLAG:
					Log("%s(%s:%s) FlagHash : %s" % (team[ip][0], ip, p, user_hash))
					exit_value = CTF_SUCCESS
				else:#Check flag
					#Check flag checks afew things
					#1. Check if the correct daemon is listening on the correct port
					#   Do not accept daemon from a diff location
					#2. Check if daemon has not been tampered with
					#3. Daemon is correctly responding
					#4. Check if hash of flagfile is valid
					
					#1 Check correctness
					#Get daemon listening on specific port to extract pid
					cmd_exec = "sudo netstat -nlp | grep LISTEN | grep :%s" % (p,)
					DebugLog(cmd_exec)
					ret = myssh.execute(cmd_exec)

					if len(ret) == 0:
						Log("Daemon %s:%s not listening" % (ip, p,))
						EnumCleanup(myssh)
						exit_value = CTF_FAIL
						continue
					
					#Take the first entry found
					pid = ret[0].rsplit()[6].split("/")[0]
					DebugLog("Found PID : %s" % pid)
					
					#Resolve PID
					cmd_exec = "sudo ls -l /proc/%s/exe" % (pid,)
					DebugLog(cmd_exec)
					ret = myssh.execute(cmd_exec)
					
					if len(ret) == 0:
						Log("Unable to resolve PID for daemon %s:%s" % (ip, p,))
						EnumCleanup(myssh)
						exit_value = CTF_FAIL
						continue
					
					#Compare running path to what we have
					ret = ret[0].split(" -> ")
					if len(ret) != 2:
						Log("!!!Wrong daemon running on %s:%s!!!" % (ip, p,))
						EnumCleanup(myssh)
						exit_value = CTF_FAIL
						continue
	
					ret = ret[1].strip("\n")
					if ret != daemon_path[p][0] + daemon_path[p][1]:
						Log("!!!Wrong daemon running on %s:%s (%s)!!!" % (ip, p, ret,))
						EnumCleanup(myssh)
						exit_value = CTF_FAIL
						continue
							
					#2 Check if FILEHASH is correct
					file_hash = getFileHash(myssh, daemon_path[p][0] + daemon_path[p][1])		
					if file_hash == CTF_INTERR:
						Log("Unable to hash daemon file on %s:%s (%s)" % (ip,p,daemon_path[p][0] + daemon_path[p][1]))
						EnumCleanup(myssh)
						exit_value = CTF_FAIL
						continue
						
					if file_hash != checksums[p][0]:
						Log("!!!Daemon hash mismatched (Returned-%s/Expected-%s)!!!" % (file_hash,checksums[p][0]))
						EnumCleanup(myssh)
						exit_value = CTF_FAIL
						continue
						
					useDefault = False
					#3 Daemon Network Validation
					#Check if we can get a valid response from the port, this is to prevent firewalling 
					try:
						mod = __import__(daemon_path[p][3])
						
						#Check if function validate_daemon exists
						if not hasattr(mod, "validate_daemon"):
							Log("!!!Unable to find function validate_daemon in %s, reverting to default callback" % (daemon_path[p][3],))
							useDefault = True
					except:
						Log("!!!Unable to import %s, reverting to default callback!!!" % (daemon_path[p][3],))
						useDefault = True
					
					if useDefault == True:
						DebugLog("Calling defaultConnCheck(%s,%s)" % (ip,p,))
						ret = defaultConnCheck(ip, p)
					else:
						DebugLog("Calling mod.validate_daemon(%s,%s,%s)" % (ip, p, flags[ip][p][0],))
						ret = mod.validate_daemon(ip, p, flags[ip][p][0])
					
					if ret != CTF_SUCCESS:
						Log("!!!Daemon network validation FAILED (%s:%s)!!!" % (ip, p,))
						EnumCleanup(myssh)
						if not useDefault == True: del mod
						exit_value = CTF_FAIL
						continue
					if not useDefault == True: del mod
						
					#4
					#Now check if the flag has been pwned
					real_hash = flags[ip][p][1]
					if real_hash != user_hash:
						Log("!!! %s(%s:%s) PWNED Client FlagHash (Returned-%s/Expected-%s)!!!" % (team[ip][0], ip, p, user_hash, real_hash))
						EnumCleanup(myssh)
						exit_value = CTF_FAIL
						continue
						
					Log("!!!Flag Matches for %s:%s!!!" % (ip, p))
					#Remove leftovers
					EnumCleanup(myssh)
					exit_value = CTF_SUCCESS
				
			elif op == CTF_GETHASHFILE or op == CTF_CHECKHASHFILE:
				file_hash = getFileHash(myssh, daemon_path[p][0] + daemon_path[p][1])		
				if file_hash == CTF_INTERR:
					EnumCleanup(myssh)
					continue
							
				if op == CTF_GETHASHFILE:
					Log("%s(%s:%s) File Hash : %s" % (team[ip][0],ip, p,file_hash))
					exit_value = CTF_SUCCESS
				else:
					#check againts what we have in file
					if file_hash == checksums[p][0]:
						Log("!!! %s(%s:%s) File Hash Matched (Returned-%s/Expected-%s)!!!" % (team[ip][0],ip,p,file_hash,checksums[p][0]))
						exit_value = CTF_SUCCESS
					else:
						Log("!!! %s(%s:%s) Invalid File Hash (Returned-%s/Expected-%s)!!!" % (team[ip][0],ip,p,file_hash,checksums[p][0]))
						exit_value = CTF_FAIL
					
				EnumCleanup(myssh)
	myssh.close()
			
def EnumClients(ip, port, op):
	global team, ports
	
	#Debug
	DebugLog("EnumClients : %s %s %s" % (ip,port,op))
	
	#Check if this is a legit port
	if port != 0 and port not in ports:
		Log("Unknown Port : %s" % (port,))
		usage()
	#Check if this is a legit IP
	if ip != None and ip != "0" and ip not in team:
		Log("Unknown IP : %s" % ip)
		usage()
	
	#Process each entry
	for t in team:
		if (ip == None or ip == "0") or t == ip:
			EnumClients2(t, port, op)
		
def InitDBConnection():
	global ctf_config
	global db_conn, db_cursor
	
	#Try to establish connection to db
	try:
		db_conn = MySQLdb.connect(host=ctf_config["DB_HOST"][1], user=ctf_config["DB_UNAME"][1], passwd=ctf_config["DB_PASSWD"][1], db="")
		DebugLog("Host : %s User : %s Passwd : %s" % (ctf_config["DB_HOST"][1],ctf_config["DB_UNAME"][1],ctf_config["DB_PASSWD"][1]))
		db_cursor = db_conn.cursor()
	except:
		Log("!!!Connection to %s failed!!!" % (ctf_config["DB_HOST"][1],))
		usage()
	
	#Switch to a spcific schema if set
	if ctf_config["DB_SCHEMA"][1] != None:
		try:
			DebugLog("Switching database to %s" % (ctf_config["DB_SCHEMA"][1],))
			db_cursor.execute("use %s" % (ctf_config["DB_SCHEMA"][1]))
		except:
			Log("!!!Database %s does not exist!!!" % (ctf_config["DB_SCHEMA"][1]))
			usage()
		
	
if __name__ == "__main__":
	if len(sys.argv) < 2:
		usage()

	Log("-------START (%d/%d/%d)-------" % (localtime()[2],localtime()[1],localtime()[0]), True)
	#Try loading configs
	#DB connection needs to be established b4 loading flags
	ParseConfig()
	InitDBConnection()
	LoadTeamData()

	option = sys.argv[1]
	
	#Allowed options to be passed to script
	dict_opt = {"-cf":CTF_CHECKFLAG,"-cx":CTF_CHECKHASHFILE,
				"-gf":CTF_GETFLAG,"-gx":CTF_GETHASHFILE,
				"-sf":CTF_SETFLAG}
				
	#Log what we executed
	Log(" ".join(sys.argv), True)
		
	if option == "-ec":
		ExamineConfigs()
	elif option in dict_opt:
		if(len(sys.argv) != 4): usage()
		EnumClients(sys.argv[2], long(sys.argv[3]), dict_opt[option])
	else:
		usage()
		
	Log("Exit code : %s" % (exit_value,), True)
	Log("-------END-------\n", True)
	
	#print exit_value
	exit(exit_value)
	