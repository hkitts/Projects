You are to complete the probs5-1-9_11.py template script that solves Problems 9 and 11 in Problem Set 5.1 on page 196.  For problem 9, you are computing an approximation to f'(0.2) using a second-order accurate central FDA method with h = 0.2 and then using Richardson Extrapolation with h = 0.1 to produce a higher-accurate approximation.  No external Python functions are needed for Problem 9.

Problem 9: Use the data in the table below to compute f'(0.2) as accurately as possible:
x    |	0  |	0.1 	  | 0.2      |	0.3       |	0.4      |
f(x) |	0  |	0.078348  |	0.138910 |	0.192916  |	0.244981 |

 

For problem 11, the template script will output  p'(0), p''(0), f'(0) and f''(0) along with the errors (|f(0)-p(0)| and |f'(0)-p'(0)|) since you are given the exact f(x) that produced the table of function values.  You are to use a polynomial p(x) of degree 3 (not a spline) for the function approximation.  The template script uses the polyfit, polyder, and polyval modules from Numpy for this problem.  You should use the tesla/hydra lab machines for this assignment if you cannot get the Numpy library loaded on your local machine.

Problem 11: Use polynomial interpolation to compute f' and f'' at x=0 using the data below.  Given that f(x)=x3-0.3x2-8.56x + 8.448, gauge the accuracy of the result.
x    |	-2.2    | -0.3   | 	0.8    | 1.9     |
f(x) |	15.180 	| 10.962 |	1.920  | -2.040  |