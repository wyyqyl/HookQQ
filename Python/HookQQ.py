from pydbg import *
from pydbg.defines import *
from IpInfo import *

import utils
import sys
import socket
import struct

ip_info = IPInfo("qqwry.dat")

def my_sendto(dbg, args):
	"""
	int sendto(
	_In_  SOCKET s,
	_In_  const char *buf,
	_In_  int len,
	_In_  int flags,
	_In_  const struct sockaddr *to,
	_In_  int tolen);
	"""
	buffer = dbg.read(args[1], args[2])
	#print dbg.hex_dump(buffer)
	if args[2] == 27 and ord(buffer[0]) == 3:
		id = ord(buffer[26]) | ((ord(buffer[25]) | ((ord(buffer[24]) | (ord(buffer[23]) << 8)) << 8)) << 8)
		sockaddr = dbg.read(args[4], args[5])
		ip = socket.inet_ntoa(struct.unpack('HH4s8s', sockaddr)[2])
		(country, area) = ip_info.getIPAddr(ip)
		print "ID: %d, IP: %s, %s" % (id, ip, country + area)
	return DBG_CONTINUE

def main():
	dbg = pydbg()
	found_qq = False
	
	for (pid, name) in dbg.enumerate_processes():
		if name.lower() == "qq.exe":
			found_qq = True
			hooks = utils.hook_container()
			
			dbg.attach(pid)
			print "[*] Attaching to qq.exe with PID: %d" % pid
			
			hook_address = dbg.func_resolve("ws2_32", "sendto")
			hooks.add(dbg, hook_address, 6, my_sendto, None)
			print "[*] ws2_32.sendto hooked at 0x%08X" % hook_address
			break

	if found_qq:
		print "[*] Hooks set, continuing process."
		dbg.run()
	else:
		print "[*] Cound not find qq.exe"
		sys.exit(-1)

if __name__ == "__main__":
	main()