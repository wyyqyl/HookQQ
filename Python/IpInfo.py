from struct import unpack
import socket

class IPInfo(object):

	def __init__(self, dbname):
		with open(dbname, 'rb') as database:
			self.data = database.read()
		
		# First 4 bytes: the offset of first index
		# Second 4 bytes: the offset of last index
		(self.first_index, self.last_index) = unpack('<II', self.data[0:8])

		# Each index is 7 bytes long
		self.index_count = (self.last_index - self.first_index) / 7 + 1

	def getString(self, offset = 0):
		''' Get country or area information '''
		
		# Each record ends with '\0'
		end = self.data.find('\0', offset)
		return self.data[offset:end]

	def getLong3(self, offset = 0):

		# Each offset is 3 bytes
		s = self.data[offset: offset + 3]
		s += '\0'
		return unpack('<I', s)[0]

	def getAreaAddr(self, offset = 0):
		''' Get area information from the given offset '''
		
		mode = ord(self.data[offset])
		if mode == 1 or mode == 2:
			# if the first byte is 1 or 2, call itself again with
			# the second bytes to forth bytes
			sub_offset = self.getLong3(offset + 1)
			return self.getAreaAddr(sub_offset)
		else:
			return self.getString(offset)

	def getAddr(self, offset, ip = 0):
		
		mode = ord(self.data[offset])

		if mode == 1:
			# Redirection mode 1
			# [IP][0x01][Country or area information offset]
			return self.getAddr(self.getLong3(offset + 1))
		elif mode == 2:
			# Redirection mode 2
			# [IP][0x02][Country information offset][Area information string]
			cArea = self.getAreaAddr(self.getLong3(offset + 1))
			aArea = self.getAreaAddr(offset + 4)
			return (cArea, aArea)
		else:
			# Normal mode [IP][Country information string][Area informatino string]
			cArea = self.getString(offset)
			aArea = self.getString(self.data.find('\0', offset) + 1)
			return (cArea, aArea)

	def find(self, ip, left, right):
		''' Binary search '''
		if right - left <= 1:
			offset = self.first_index + right * 7
			new_ip = unpack('<I', self.data[offset: offset + 4])[0]
			if ip == new_ip:
				return right
			else:
				return left

		middle = (left + right) / 2
		offset = self.first_index + middle * 7
		
		new_ip = unpack('<I', self.data[offset: offset + 4])[0]
		
		if ip < new_ip:
			return self.find(ip, left, middle - 1)
		elif ip > new_ip:
			return self.find(ip, middle, right)
		else:
			return middle
		
	def getIPAddr(self, ip):
		if ip.startswith("255"):
			return ("", "")
		ip = unpack('!I', socket.inet_aton(ip))[0]
		index = self.find(ip, 0, self.index_count - 1)
		offset = self.first_index + index * 7
		sub_offset = self.getLong3(offset + 4)
		(country, area) = self.getAddr(sub_offset + 4)
		if area == " CZ88.NET":
			area = ""
		elif len(area) == 2:
			if ord(area[0]) == 2 and ord(area[1]) == 25:
				area = ""
		return (country, area)
		
if __name__ == "__main__":
	ip_info = IPInfo("qqwry.dat")
	(country, area) = ip_info.getIPAddr("127.0.0.1")
	print country + area
	
	# with open("ip.txt", "rb") as ipfile:
		# data = ipfile.readlines()
		# count0 = 0
		# count1 = 0
		# for item in data:
			# if "CZ88.NET" in item:
				# offset = item.find(" ")
				# ip = item[:offset]
				# (country, area) = ip_info.getIPAddr(ip)
				# if len(area) == 2:
					# count0 += 1
					# if ord(area[0]) != 2 or ord(area[1]) != 25:
						# print "ip: %s" % ip
				# elif len(area) == 0:
					# pass
				# else:
					# if area != " CZ88.NET":
						# print "ip: %s" % ip
					# else:
						# count1 += 1
		# print count0, count1
