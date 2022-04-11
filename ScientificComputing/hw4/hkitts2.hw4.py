#!/usr/bin/env python3
# prob2-3-19 (hwk #4)
#
# Hunter Kitts
#
#  Problem 2.3.19 (pg. 101)
#
#  1) Implement the Ax(v) function below so that it will initialize
#     and return the matrix-vector product Ax, where A is given at
#     the top of page 101 in the textbook.
#  
#  2) Use the provided conjGrad.py module to solve the system described
#     in problem #19 (uncomment and fill in the '??' lines and provide
#     the requested loop and final error difference).
#
# The correct solution is:
#
# [ 21.42857143 38.39285714  57.14285714   47.32142857   75.
#   90.17857143 92.85714286 124.10714286 128.57142857]
#
########################################################################

from numpy import zeros,array,linalg
from conjGrad import *

# Refer to pg. 101 for what the matrix Ax should be
def Ax(v):
  Ax = zeros((9))*1.0
  Ax[0] = -4*v[0] + v[1] + v[3]
  Ax[1] = v[0] + -4*v[1] + v[2] + v[4]
  Ax[2] = v[1] + -4*v[2] + v[5]
  Ax[3] = v[0] + -4*v[3] + v[4] +v[6]
  Ax[4] = v[1] + v[3] + -4*v[4] + v[5] +v[7]
  Ax[5] = v[2] + v[4] + -4*v[5] + v[8]
  Ax[6] = v[3] + -4*v[6] + v[7]
  Ax[7] = v[4] + v[6] + -4*v[7] + v[8]
  Ax[8] = v[5] + v[7] + -4*v[8]
  # fill in the 9 rows of Ax
  return Ax

b = array([0,0,100,0,0,100,200,200,300])*(-1.0)
x = zeros((9))*1.0
tol = 1e-06
s1,numIter = conjGrad(Ax,x,b,tol)
print("\nThe solution is:\n",s1)
print("\nNumber of iterations =",numIter, "using Tol: ", 1e-06)

print("\nCG Convergence Test")
print("Iterations   Tolerance")

tol_list = [1.e-02, 1.e-04, 1.e-06, 1.e-08, 1.e-10, 1.e-12, 1.e-14, 1.e-16]

# Create a loop here to call conjGrad with these tolerances and obtain/print the iteration
# count with each tolerance used:  1.e-02, 1.e-04, 1.e-06, 1.e-08, 1.e-10, 1.e-12, 1.e-14, 1.e-16
#
for t in tol_list:
	s2,numIter = conjGrad(Ax,x,b,t)
	print('{:<10}   {:.1e}'.format(numIter, t))
print('')

# Finally, print the error between the solutions using the 1.e-06 and 1.e-16 tolerances
print("\nError between vectors obtained with tol=1e-06 and tol=1e-16: ",linalg.norm(s1-s2),"\n")
