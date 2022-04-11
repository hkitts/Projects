Using the template, prob4-1-27.py above you are to solve a 3 by 3 non-linear system of equations (via newtonRaphson2.py) to determine the smallest R of the trajectory and the corresponding angle theta for the orbiting satellite based on Problem 27 of Problem Set 4.1 (pg. 171) of the textbook.  You should define the unknowns as x[0] = C, x[1] = e, and x[2] = alpha.  The smallest R in kilometers will occur when sin(theta + alpha) = 1, and the corresponding theta for this smallest R is given by (pi/2.0 - x[2]) * 180.0 / pi in degrees. The minimum trajectory has the value C/(1+e) and occurs at sin (θ+α)=1 or θ=π/2 - α.  Your initial guess for the vector (C,e,α) should be (6800,0.5,0).  

The required outputs (to stdout) are:

    The minimum trajectory.
    The angle (θ) in degrees that the minimum trajectory occurs at.
    The values for C,e, and α.

and a polar-coordinate graph similar to the one shown in polargraph.png (see above) depicting the full orbit of the satellite should be produced (in a separate window).