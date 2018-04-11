import os
import sys
import csv
import time
from subprocess import check_output

## varialbs
process_dir = "/home/kl186/ros1_evaluation/devel/lib/tcpros_eval/"
subscribe_dir = "/home/kl186/ros1_evaluation/evaluation/subscribe_time/"
transport_dir = "/home/kl186/ros1_evaluation/evaluation/transport_time/"
listener_dir = '/home/kl186/ros1_evaluation/src/tcpros_eval/src/'
listener_file = 'listener_tcpros_single.cpp'

## Function

def get_pid(name):
	return check_output(['pidof',name])

# generate 5 different listners
for n in range(1,6):
	os.system('cp ' + listener_dir + listener_file + ' ' + listener_dir+ 'test'+str(n)+'_'+listener_file)

# change the output files of the 5 listeners
for n in range(1,6):
	oldfile = open(listener_dir+listener_file, 'r')
	
	filelist = oldfile.readlines()
	targetline = filelist[32]
	newline = targetline[0:59]+'test'+str(n)+'_'+targetline[59:]
	filelist[32] = newline
	
	newfile = open(listener_dir+ 'test'+str(n)+'_'+listener_file, 'w')
	for line in filelist:
		newfile.write(line)
	newfile.close()

os.system('catkin_make')

# # setup 5 listeners and 1 publisher
for n in range(1,6):
	run_cm = 'rosrun tcpros_eval ' + 'test'+str(n)+'_'+'listener_tcpros_single __name:=node'+str(n)+' &'
	os.system(run_cm)

time.sleep(2)
os.system('rosrun tcpros_eval talker_tcpros_single &')

os.system('sleep 30')

for n in range(1,6):
	listener_name = process_dir+'test'+str(n)+'_'+'listener_tcpros_single __name:=node' + str(n)
	pid = int(get_pid(listener_name))
	os.sytem('kill %d'%pid)

# # remove all generated listners
# os.system('rm ' + listener_dir + 'test*')