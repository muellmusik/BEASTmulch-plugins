/*
 BEASTmulch UGens
 Copyright (C) 2009 Scott Wilson
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 2 as published by
 the Free Software Foundation.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA 
 
 http://www.beast.bham.ac.uk/research/mulch.shtml
 beastmulch-info@contacts.bham.ac.uk
 
 The BEASTmulch project was supported by a grant from the Arts and Humanities Research Council of the UK: http://www.ahrc.ac.uk 

*/

#if defined( __GNUC__ )
//#include<ppc_intrinsics.h>
#endif
#include <math.h>
#include <Accelerate/Accelerate.h>
#include <stdint.h>

#if defined(__ppc__) || defined(__ppc64__)

inline float FastScalarInvSqrt( float f ) 
{
	
	float estimate, estimate2;
	float oneHalf = 0.5f;
	float one = oneHalf + oneHalf;
	
	//Calculate a 5 bit starting estimate for the reciprocal sqrt
	estimate = estimate2 = __frsqrte ( f );
	
	//if you require less precision, you may reduce the number of loop iterations
	estimate = estimate + oneHalf * estimate * ( one - f * estimate * estimate );
	estimate = estimate + oneHalf * estimate * ( one - f * estimate * estimate );
	estimate = estimate + oneHalf * estimate * ( one - f * estimate * estimate );
	
	return __fsels( -f, estimate2, estimate );
	
}


#endif // __ppc__ || __ppc64__

// safe for zero and negative numbers, but not for Nan
inline float FastScalarSqrt( float f ) {
	//checkBadValues(f);
	float returnval;
// fast version for PPC	
#if defined(__ppc__) || defined(__ppc64__)
	returnval = f == 0.f ? 0.f :  f * (f < 0.f ? FastScalarInvSqrt(-f) : FastScalarInvSqrt(f));
#else
	returnval = f == 0.f ? 0.f : sqrtf(fabsf(f)); // fast enough on intel, and faster than anything else...
	//returnval = f == 0.f ? 0.f :  f * (f < 0.f ? FastScalarInvSqrt(-f) : FastScalarInvSqrt(f));
#endif // __ppc__ || __ppc64__
	
	return returnval;
}

// Vector Version
#if __VEC__
inline vector float vecReciprocalSquareRoot( vector float v )
{
	//Get the square root reciprocal estimate
	vector float zero = (vector float)(0);
	vector float oneHalf = (vector float)(0.5);
	vector float one = (vector float)(1.0);
	//vector float absv = vec_abs(v); // safe for negative values
	vector float estimate = vec_rsqrte( v );
	
	// assume inf was 0 and correct using one's complment of compare mask
	estimate = vec_andc(estimate, vec_cmpeq(estimate, (vector float)(INFINITY)));
	
	//3 rounds of Newton-Raphson refinement
	vector float estimateSquared = vec_madd( estimate, estimate, zero );
	vector float halfEstimate = vec_madd( estimate, oneHalf, zero );
	estimate = vec_madd( vec_nmsub( v, estimateSquared, one ), halfEstimate, estimate );
	
	estimateSquared = vec_madd( estimate, estimate, zero );
	halfEstimate = vec_madd( estimate, oneHalf, zero );
	estimate = vec_madd( vec_nmsub( v, estimateSquared, one ), halfEstimate, estimate );
	
	estimateSquared = vec_madd( estimate, estimate, zero );
	halfEstimate = vec_madd( estimate, oneHalf, zero );
	return vec_madd( vec_nmsub( v, estimateSquared, one ), halfEstimate, estimate );
	
}

inline vector float vecSquareRoot( vector float v )
{
	return vec_madd( v, vecReciprocalSquareRoot( v ), (vector float)(0) );
}

#endif
