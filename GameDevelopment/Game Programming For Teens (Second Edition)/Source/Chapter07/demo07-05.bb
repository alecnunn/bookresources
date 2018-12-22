;demo07-05.bb - Demonstrates Scaling Lines in a downward fashion.

Graphics 800,600,0,2 ;set up graphics



;STRUCTURES
;The point type defines a single local coordinate position
Type point
	Field x,y
End Type 

;Create the three vertices of the triangle
point1.point = New point
point2.point = New point
point3.point = New point




;VARIABLES
;These variables define each vertex and are in local coordinates
point1\x= 0
point1\y= 0
point2\x= 100
point2\y= 100
point3\x= -100
point3\y = 100

;CONSTANTS
;The global indicators that is added to each local coordinate to place it on screen
Const xs = 400
Const ys = 300


;MAIN SECTION

Print "This is our first triangle." 

;Draw out first triangle
Line point1\x + xs, point1\y + ys, point2\x + xs, point2\y + ys
Line point2\x + xs, point2\y + ys, point3\x + xs, point3\y + ys
Line point3\x + xs, point3\y + ys, point1\x + xs, point1\y + ys

Flip

;Find scaling factor from user
sxy# = Input ("What would you like the scaling factor to be? (Ex: 50% = .5)? ==> ") 

Flip

;Multiply all the coordinates by the scaling factor
point1\x = point1\x * sxy#
point1\y = point1\y * sxy#
point2\x = point2\x * sxy#
point2\y = point2\y * sxy#
point3\x = point3\x * sxy#
point3\y = point3\y * sxy#
	


;Change the default color to green
Color 0,255,0

;Draw final triangle (with scaled coordinates) in green
Line point1\x + xs, point1\y + ys, point2\x + xs, point2\y + ys
Line point2\x + xs, point2\y + ys, point3\x + xs, point3\y + ys
Line point3\x + xs, point3\y + ys, point1\x + xs, point1\y + ys

Print "Press any key to exit." 

;Wait for user to press a key before exiting
WaitKey 