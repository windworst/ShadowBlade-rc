import os

class cfilecount:
	all_file = []
	usable_file = []
	total_line = 0
	total_size = 0
	def __init__(self,path,list):
		self.all_file = []
		self.all_file.append(path)
		for file in iter(self.all_file):
			try:
				dirlist = os.listdir(file)
				for dir in dirlist:
					self.all_file.append(file + '/' + dir)
			except:
				for filter in list:
					if filter == file[-(len(filter)):len(file)]:
						size = 0
						line = 0
						try:
							f = open(file,'r')
							data = f.read()
							size = len(data)
							line = len(data.split('\n'))
						except:
							pass
						self.total_size += size
						self.total_line += line
						info = file[2:len(file)],line,size
						self.usable_file.append(info)
						break

if __name__=='__main__':
	c = cfilecount('.',['.c','.cpp','.h'])
	try:
		f = open("cfilecount.txt","w")
		for file,line,size in c.usable_file:
			f.write( '%s line: %d,size: %d\n'%(file,line,size) )
		f.write('Total:  line:%d,size:%d\n'%(c.total_line,c.total_size))
	except:
		pass
