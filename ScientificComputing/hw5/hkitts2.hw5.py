#!/usr/bin/env python4
#
# Hunter Kitts HW5 Interpolation using Cubic Splines 
#
# Template for Problems 17 and 19 in Problem Set 3.1 (Hwk #5)     p.25 in pdf
#
# Problem 17 (use log-log scale)
#
from cubicSpline import *
from numpy import array,log10
xData = array([0.2, 2.0, 20.0, 200.0, 2000.0, 20000.0])
yData = array([103.0, 13.9, 2.72, 0.8, 0.401, 0.433])
Re_array= array([5.0, 50.0, 500.0, 5000.0])
logxData = log10(xData)
logyData = log10(yData)
#
# Generate curvatures here
#
k = curvatures(logxData, logyData)

print('\nInterpolating for drag coefficients (C_D)...\n    Re\tC_D')

#
# Create a loop to evaluate the Cubic Spline for the elements
# in the Re_array but at logscale (using the log10 function)
#
logRe = log10(Re_array)
n = len(logRe)

for i in range(0,n):
	logy = evalSpline(logxData, logyData, k, logRe[i])
	print('{:6.1f}\t{:5.3f}'.format(Re_array[i],10.0**logy))

#for Re in logRe:
	#logy = evalSpline(logxData,logyData,k, Re)
	# Use this print statement in your loop ...
	#print('{:6.1f}\t{:5.3f}'.format(Re,10.0**logy)) # Re is an element from
                                                  # the RE_array
                                                  # logy is the y-coordinate
                                                  # corresp. to log10(Re)
#
# Problem 19 (do not use log-log scale)
#
tData = array([0.0, 21.1, 37.8, 54.4, 71.1, 87.8, 100.0])
mData = array([1.79, 1.13, 0.696, 0.519, 0.338, 0.321, 0.296]) 
T_array = array([10.0, 30.0, 60.0, 90.0])
#
# Generate curvatures here
#
k = curvatures(tData, mData)

print('\nInterpolating for kinematic viscosity (CMu_k)...\n    T\tMu_k')

#
# Create a loop to evaluate the Cubic Spline for the elements
# in the T_array
#
# Use this print statement in your loop ...
n = len(T_array)

for i in range(0,n):
	y = evalSpline(tData, mData, k, T_array[i])
	print('{:6.1f}\t{:5.3f}'.format(T_array[i],y)) # temp is an element from 
                                           # T_array
