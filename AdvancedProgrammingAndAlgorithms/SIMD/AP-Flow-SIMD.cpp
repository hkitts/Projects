/* Hunter Kitts Lab5 cs494 */
#include "AP-Flow.h"
#include <emmintrin.h>

//this was used for testing
void print_simd(string s, __m128i v)
{
	uint8_t array[16];
	__m128i *c;
	int i;

	c = (__m128i *) array;
	*c = v;
	printf("%-9s", s.c_str());
	for (i = 0; i<16; i++)printf("%4d", array[i]);
	printf("\n");
}


void APFlow::CalcFlow()
{
	int i, j, v, f;
	__m128i alli, fv, rv;   //these are new vectors which have the same values
							//as something in Flow (storing Flow so it can change itself)

	__m128i *vv;			//just points to location in Flow
	__m128i *iv;
	__m128i *tmp;			//used for last for loop


	int t;

	for (i = 0; i < N*N; i++) Flow[i] = Adj[i];

	for(v = 0; v < N; v++){
		for(i = 0; i < N; i++){
			/* Create a vector alli, which is 16 instances of Flow[i*N+v] */
			alli = _mm_set1_epi8(Flow[i*N+v]);

			//if(i == 3 && v == 2) print_simd("alli", alli);

			for(j = 0; j < N; j += 16){

				/* Load Flow[v*N+j] through Flow[v*N+j+15] to vector vv */
				vv = (__m128i *) &Flow[v*N+j];
				//if(i == 3 && v == 2) print_simd("vv", *vv);	
				/* Create fv, which is the flow from i to each of j through j+15
				   through v. This is simply the min of alli and vv. */
				fv = _mm_min_epu8 (alli, *vv);
				//if(i == 3 && v == 2) print_simd("fv", fv);

				/* Load Flow[i*N+j] through Flow[i*N+j+15] to vector iv */
				iv = (__m128i *) &Flow[i*N+j];
				//if(i == 3 && v == 2) print_simd("iv", *iv);

				/* Create rv, which is the max of each value of fv and iv. */
				rv = _mm_max_epu8 (fv, *iv);
				//if(i == 3 && v == 2) print_simd("rv", rv);

				/* Store rv into Flow[i*N+j] through Flow[i*N+j+15] */
				tmp = (__m128i *) &Flow[i*N+j];
				*tmp = rv;
			}
		}
	}
}
