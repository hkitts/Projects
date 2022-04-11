#!/usr/bin/env python3
####################################################################
# problem2_1_15  (Problem Set 2.1, No. 15 on page 53 of textbook)
# This problem requires the LUdecomp.py module (provided).
#
# Hunter Kitts
# HW2 COSC370/377 2020
####################################################################

from numpy import zeros, ones, array, float64, inf
from numpy import linalg
from LUdecomp import *

norm = linalg.norm  # Python allows you to rename functions.
                    # Here, 'linalg.norm' is renamed to just 'norm'.

# until the err is greater than the total keep increasing the size of
# the hilbert matrix
TOL = 0.000001		#Tolerance set to 1.E-6
err = 0
n = 0
while err < TOL:
  n +=1
  a = zeros((n,n),dtype=float64)
  b = zeros((n),dtype=float64)
  soln = ones((n), dtype=float64) # The correct solution is all 1's

  # Use the loops below to define the matrix 'a' and vector 'b':
  for i in range(n):
    for j in range(n):
      a[i,j] = 1/(i+j+1.0)	#set a to hilbert matrix
      b[i] = b[i] + a[i,j]	#set b to sum of hilbert matrix
							#each index in b is sum of row in a

  # Call appropriate functions from the LUdecomp.py module to
  # solve the equations A x = b with the b-vector being overridden
  # by the solution vector x.

  a = LUdecomp(a)		#LU decomposition on a
  b = LUsolve(a,b)		#solves for x where [a]{x} = b

  # Your solution should be stored in 'b' (if you used a 
  #  different variable name, modify the code below accordingly).
  #
  print("\n", n, " equations. ", "The solution is:\n", b)
  err = norm(b-soln, ord=inf)
  print("Error (inf-norm of difference)): ", err)

print("^^^(Greater than TOL = ", TOL, ")^^^\n")
print("********************************************\n")
print("Max number of equations while error remains less than ", TOL, " is: ", n-1, "\n") 
print("********************************************")
