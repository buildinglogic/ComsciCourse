import os
import sys
import time
import csv
import numpy as np
from subprocess import check_output

## Variable

test_msg_size = '_time_2Mbyte.txt'
process_name = "/home/kl186/ros1_evaluation/devel/lib/tcpros_eval/listener_tcpros_single"
subscribe_dir = "/home/kl186/ros1_evaluation/evaluation/subscribe_time/"
transport_dir = "/home/kl186/ros1_evaluation/evaluation/transport_time/"
target_dir = './src/tcpros_eval/src/'
target_file = 'talker_tcpros_single_bk.cpp'
new_file = 'talker_tcpros_single.cpp'

# generate hz_list for examination
hz_list = []
for hz in range(10, 1000, 90):
	hz_list.append(hz)

## Function

def get_pid(name):
	return check_output(['pidof',name])

output_file = open('data_loss_transport_time_hz.csv', 'w')
csv_out = csv.writer(output_file)


## loop to change hzs
for new_hz in hz_list:
	## Step1: change hz value

	oldfile = open(target_dir + target_file,'r')

	filelist = oldfile.readlines()
	targetline = filelist[14]
	newline = targetline[0:19] + str(new_hz) + '\n'
	filelist[14] = newline

	newfile = open(target_dir + new_file,'w')
	for line in filelist:
		newfile.write(line)

	oldfile.close()
	newfile.close()

	## Step2: Compile

	os.system('catkin_make')

	## Step3: Runing listener and talker

	os.system('rosrun tcpros_eval listener_tcpros_single &')
	os.system('sleep 2')
	os.system('rosrun tcpros_eval talker_tcpros_single &')

	## Step4: Process data

	os.system('./cal')

	## Step5: Store missing data

	file = open(subscribe_dir+"subscribe"+test_msg_size,'r')

	line1 = file.readlines()[0]
	miss_num = int(line1)

	file.close()

	## Step6: Store median data

	transport_array = np.loadtxt(transport_dir+"transport"+test_msg_size)

	median_num = np.median(transport_array)

	# write output
	csv_out.writerow([new_hz, miss_num, median_num])

	# kill listener to ready for next loop
	time.sleep(10)
	pid = int(get_pid(process_name))
	os.system('kill %d'%pid)

output_file.close()

# print "Fianl result\n"
# print "miss_num: %d"%miss_num
# print "median_num: %f"%median_num



