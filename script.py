#Python script for generating the results of experiments and drawing the plots
#For executing command line arguments I used os.system()

import os
import subprocess
import matplotlib.pyplot as plt
capacities = [1,2,4,8,16,32]
associativities = [1,2,4,8,16]
block_sizes = [8,16,32,64,128]

#Compile
os.system("make")
#Experiment 1
set_sizes1 = [i * 1024 / 32 for i in capacities]
miss_rates1 = []
for set_size in set_sizes1:
	out = os.popen("./cache_sim " + str(set_size) + " 1 32 ls.trace").read()
	arr = [int(s) for s in out.split() if s.isdigit()]
	miss_rates1.append(float(arr[1])/float(arr[0]))
plt.plot(capacities, miss_rates1, marker = 'o')
plt.xlabel("Capacity")
plt.ylabel("Miss Rate")
plt.savefig("Capacity_vs_Miss_Rate.png")

#Experiment 2
set_sizes2 = [int(8 * 1024 / 32) // int(i) for i in associativities]
miss_rates2 = []
for i, set_size in enumerate(set_sizes2):
	out = os.popen("./cache_sim " + str(set_size) + " " + str(associativities[i]) + " 32 ls.trace").read()
	arr = [int(s) for s in out.split() if s.isdigit()]
	miss_rates2.append(float(arr[1])/float(arr[0]))
plt.gcf().clear()
plt.plot(associativities, miss_rates2, marker = 'o')
plt.xlabel("Associativity")
plt.ylabel("Miss Rate")
plt.savefig("Associativity_vs_Miss_Rate.png")


#Experiment 3
set_sizes3 = [int(8 * 1024 / i) for i in block_sizes]
miss_rates3 = []
for i, set_size in enumerate(set_sizes3):
	out = os.popen("./cache_sim " + str(set_size) + " 1 " + " " + str(block_sizes[i]) + " ls.trace").read()
	arr = [int(s) for s in out.split() if s.isdigit()]
	miss_rates3.append(float(arr[1])/float(arr[0]))
plt.gcf().clear()
plt.plot(block_sizes, miss_rates3, marker = 'o')
plt.xlabel("Cache line size")
plt.ylabel("Miss Rate")
plt.savefig("Block_size_vs_Miss_Rate.png")


