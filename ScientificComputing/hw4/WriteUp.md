Using the prob-2-3-19.py template and conjGrad.py script, solve the heat equation on the nine interior mesh points. See the pdf file for the problem given in the textbook (Problem Set 2.3 #19).  Notice that the conjGrad function uses a function Av( ) for matrix-vector multiplication.  You are to code such a function Av() that exploits the sparsity of the matrix A (i.e, you should only reference the nonzeros of each row of the matrix A). Point deductions will be made for coding the function Av as a dense matrix-vector multiplication. The function calling conjGrad can provide any named function for the purpose of matrix-vector multiplication (it does not have to be named Av). Your output should be a table of iterations required for various convergence tolerances and the error between solutions using 1.E-06 and 1.E-16 tolerances.  The table (written to stdout) should be similar to the one shown below.   Note that the iteration count (in each case) must be determined by the conjGrad function (output parameter) and not simply typed into the required print statement.

CG Convergence Test
Iterations Tolerance
       4        1.0e-02
       4        1.0e-04
       4        1.0e-06
       4        1.0e-08
       4        1.0e-10
       4        1.0e-12
       8        1.0e-14
       8        1.0e-16