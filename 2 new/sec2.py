import re
import sys
import os

def main():
	if len(sys.argv) > 1:
		t = ''
		for i in range(1,len(sys.argv)):
			if os.path.exists(sys.argv[i]):
				try:
					try:
						f = open(sys.argv[i],'r')
						t += f.read()
						f.close()
					except IOError as e:
						print "This file can't be open!"
					t = "\n" + t + "\n "
					t = t.replace(" ",'\n')
				except MemoryError as e:
					print "Not enough memory!"
			else:
				print "Missing file!"
				continue
		regExp = re.compile('^\s*(\d+)\s*$',re.MULTILINE)
		numbers = regExp.findall(t) 
		numbers.sort(key=int)
		f = open('output.txt','w')
		for i in range(len(numbers)):
			f.write(numbers[i] + "\n")
		f.close()
		print "All done."	
	else:
		print "Input more files please!"
		print "Example: sec.py file1 [file2]"
		print "file1 ... filen-1 - data files, output.txt - output file"
main()