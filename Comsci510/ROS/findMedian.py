import os
import sys
import csv # write output into a csv file
import numpy as np
from subprocess import check_output # find process pid from its name

## variable
home_dir = '/home/kl186/ros1_evaluation/'
transport_time_dir = home_dir + 'evaluation/transport_time/'
listner_process_name = "/home/kl186/ros1_evaluation/devel/lib/tcpros_eval/listener_tcpros"

## Function

def get_pid(name):
	return check_output(['pidof',name])


## evaluation: run listener and talker
os.system('catkin_make')
os.system('rosrun tcpros_eval listener_tcpros &')
os.system('sleep 2')
os.system('rosrun tcpros_eval talker_tcpros &')
os.system('sleep 300')
pid = int(get_pid(listner_process_name))
os.system('kill %d'%pid)

os.system('./cal')

## analysis find median

# generate file list
sizes = ['256','512','1K','2K','4K','8K','16K','32K','64K','128K','256K','512K','1M','2M','4M']

# list of tuple to store size and median
median_lists = [['size', 'median']]

for size in sizes:
	data = np.loadtxt(transport_time_dir + 'transport_time_' + size + 'byte.txt')
	m = np.median(data)
	median_lists.append([size, m])

with open('medians_transport.csv', 'w') as output_file:
	csv_out = csv.writer(output_file)
	for line in median_lists:
		csv_out.writerow(line)



