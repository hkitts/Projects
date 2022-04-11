#!/usr/bin/env python3
#
# Template script for Problem 14 from Problemt Set 6.1 (p. 214 of textbook)
#  
# I this comment block, briefly explain the procedure referred to as
# Romberg Integration and provide your name and assignment number.
#  
# Hunter Kitts  
# Homework 9: Romberg Integration for Heat Capacity 
#
# Romberg Integration uses a combination of trapezoidal rule and Richardson 
# extrapolation to appoximate the integral
# 
# For example take an approximation with trapezoidal rule  with 1 panel
# Do the same estimate with 2 panels
#
# Then using Richardson extrapolation we use the two approximation to remove
# the error
#
# This can be conntinued to get a better approximation by doubling the panels
# and using Richardson extrapolation
#
from romberg import *
from numpy import *
import matplotlib.pyplot as plt
#import pylab

def f(x):
  if x == 0: return 0
  else: return  ((x**4) * (exp(x)))/(((exp(x)) -1)**2)   # define the integrable function here

u = arange(0,1.01,0.05)
print ("    u\t   g(u)")
gu = [] #list that will contain all of g(u)s (y-coordinates for your plot)

for i in u:
  if i == 0: g = 0.0;
  else:
    I,nPanels = romberg(f,0,(1/i))  # perform romberg integration on f here (get result in
                                    # I, nPanels is the number of panels used but is not
                                    # used for output.
    g = i**3 * I                    # evaluate g(i) here
  print ('{:6.2f}{:13.6f}'.format(i,g))
  gu.append(g)

plt.figure()
plt.plot(u, gu, 'b-')
plt.xlabel("u")
plt.ylabel("g(u)")
plt.title("Problem 6.1.14")
plt.xlim(0.0,1.0) 
plt.ylim(0.00,0.35)
plt.savefig("prob6_1_14.png")
#pylab.show()

#
# Place the code that creates the required plot using pylab here.
# Be sure to label axes and provide the same title as shown
# in the "prob6_1-14.png" image file on BB
#
# Table written to stdout for verification purposes:
#
#  u	     g(u)
# 0.00     0.000000
# 0.05     0.003247
# 0.10     0.025274
# 0.15     0.070997
# 0.20     0.122878
# 0.25     0.167686
# 0.30     0.202568
# 0.35     0.228858
# 0.40     0.248618
# 0.45     0.263608
# 0.50     0.275136
# 0.55     0.284136
# 0.60     0.291265
# 0.65     0.296992
# 0.70     0.301651
# 0.75     0.305487
# 0.80     0.308678
# 0.85     0.311359
# 0.90     0.313631
# 0.95     0.315573
# 1.00     0.317244
