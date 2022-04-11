Complete and submit the template provided for problems 12, 17, and 18 from Problem Set 2.2 on pages 79-82 of the text.  These problems are provided in the pdf files listed below.

Hints (and Corrections): For Problem 12, you may assume k=1 and W=1 and define the kis and Wis as specified in the problem description. Some books may have the last equation for Problem 17 incorrect.  It should be -30i1-15i2+45i3=120. 

For Problem 12, you can define the first diagonal element a[0.0]=k[0]+k[1]+k[2}+k [4] using zero-based indexing with the array k=[1,2,1,1,2] - this reflects the stiffness of the 2nd and the 5th springs being twice that of the other three.  Similarly, a[0,1]=a[1,0]=-k[2]=-1.  You can then populate the rest of the coefficient matrix this way and use [2,1,2] for the right-hand-side vector as the solution vector will have units W/k. 

In Problem 18, the top right terminal should be 0V not +120V so that that the voltage differential across the terminals is -120V (counter-clockwise). To clarify the application of Kirchoff's Law, the resulting equations are provided here:  50(i1-i2)+30(i1-i3)=-120;  50(i2-I1)+15i2+25(i2-i4)+10(i2-i3)=0; 30(i3-i1)+10(i3-i2)+20(i3-i4)+5i3 =0; 20(i4-i3)+25(i4-i2)+(10+30+15)i4 = 0.