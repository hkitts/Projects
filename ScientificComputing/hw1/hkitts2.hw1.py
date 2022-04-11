#!/hkitts2/bin/env python3
# Hunter Kitts - hkitts2 - Homework 1
# July 2013 High Temps for Knoxville, TN
# Hwk 1 for COSC 370
#
from numpy import arange
import matplotlib.pyplot as plt
xData = arange(1,32)    # Ranges for x and y axes must match
tData = [86,87,84,86,86,86,84,83,90,89,88,85,86,79,83,81, \
		75,80,81,85,81,88,89,87,84,85,86,88,88,90,90]
avg = [86.]  # First value for montly avg high temp is just Day 1 temp



i = 0               #count index of tData
total = 0;          #running total of tData

#append averages of tData into avg
for x in tData:
	total = total + x
	i = i + 1
	if(i > 1):
		avg.append(total/i)

#print table for data
print('+---------+-----+')
print('| Average | Day |')
print('+---------+-----+')
i = 1
#print avg values and date (i)
for x in avg:
	print('|   ', end=' ')
	print('{:.1f}'.format(x), end= ' ')
	if(i < 10 ):
		print('| ',i,' |')
	else:
		print('| ',i,'|')
	print('+---------+-----+')
	i = i + 1

#plot data for graph
plt.plot(xData, tData, 'bo-', markerfacecolor= 'red')			#plot xData & tData
plt.plot(avg, 'g--')											#plot avg
plt.text(15.25, 86, "Monthly Avg", color='green', fontsize=12)	#label avg graph
plt.xlim(0,32)													#set x & y axis
plt.ylim(70,95)
plt.xlabel('Day')												#label x & y axis
plt.ylabel('High Temp')
plt.grid(color = 'grey', ls = ':')								#add grid
plt.title('High Temperatures for Knoxville, TN - August 2013')	#draw title
plt.show()														#show the graph
