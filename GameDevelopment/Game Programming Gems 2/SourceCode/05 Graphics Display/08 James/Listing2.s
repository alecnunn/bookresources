; Copyright (C) Greg James, 2001. 
; All rights reserved worldwide.
;
; This software is provided "as is" without express or implied
; warranties. You may freely copy and compile this source into
; applications you distribute provided that the copyright text
; below is included in the resulting source code, for example:
; "Portions Copyright (C) Greg James, 2001"
;
// Creates an RGB normal map from an input grayscale height map
// Pairing of RGB and Alpha instructions is not used
//   Normal map parameterization is [0,1] so 0.5 = zero component
//   along that axis (value of 127 stored in the texture).
//     Red   = positive S axis
//     Green = positive T axis
//	   Blue  = positive R axis (up out of page)

// Declare pixel shader version
ps.1.1


def c5, 1.0, 0.0, 0.0, 1.0	// red mask for s axis component
def c6, 0.0, 1.0, 0.0, 1.0	// green mask for t axis component
def c4, 0.0, 0.0, 1.0, 1.0	// blue mask for r axis component
							// (blue = up out of texture)
def c2, 0.5, 0.5, 0.0, 0.0	// 0.5 bias for red & green
def c1, 1.0, 1.0, 0.0, 0.0	// color mask for red & green


; get colors from all 4 texture stages
; t0 = -s,  0
; t1 = +s,  0
; t2 =  0, +t
; t3 =  0, -t


// Select source grayscale texture into all 4 texture stages
// Sample all 4 texture stages
tex t0			// t0 = RGBA texel at coordinate offset by (-s, 0)
tex t1			// t1 = (+s, 0)
tex t2			// t2 = ( 0,+t)
tex t3			// t3 = ( 0,-t)

sub_x4 r0, t0, t1	// r0 = (t0-t1)*4 = s axis height slope
					//  Use _x4 to increase low contrast grayscale input
mul    t0, r0, c5	// t0 = r0 * red mask = red component only
					// Use t0 as temp storage 

sub_x4 r1, t3, t2		// r1 = (t3-t2)*4 = t axis height slope
mad    r0, r1, c6, t0	// r0 = r1.green + t0  = s and t result in red,green
mul    t1, r0, r0		// t1 = square s and t components
						// Use t1 as temporary storage

dp3_d2 r1, 1-t1, c1	// r1.rgb = (1 - s^2 + 1 - t^2 )/2
					// (1-s^2) is approx sqrt(1-s^2) for small s
add r0, r0, c2		// r0 = r0 + 0.5 red + 0.5 green
					// shifts signed values to [0,1]

mad r0, r1, c4, r0	// RGB = (r+0, g+0, 0+blue )
					// output = r0

