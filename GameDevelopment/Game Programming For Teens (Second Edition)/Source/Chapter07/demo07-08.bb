;demo07-08.bb - Demonstrates Scaling with ScaleImage
Graphics 800,600,0,2 

;Make sure AutoMidHandle is true
AutoMidHandle True

;STRUCTURES
;The point structure defines one coordinate point
Type point
	Field x,y
End Type 

;Create the three vertices
point1.point = New point
point2.point = New point
point3.point = New point




;VARIABLES
;These variables are in local coordinates and define the positions of the vertices
point1\x= 100
point1\y= 0
point2\x= 200
point2\y= 200
point3\x= 0
point3\y = 200

;Create a buffer with the proper height and width
image = CreateImage( (point2\x - point3\x) + 1, (point2\y - point1\y) + 1 )


;MAIN SECTION

Print "This is our first triangle." 

;Set default buffer to the image we created so that we can draw the triangle directly to it
SetBuffer ImageBuffer(image)

;Draw the triangle on the new buffer
Line point1\x, point1\y, point2\x, point2\y
Line point2\x, point2\y, point3\x, point3\y
Line point3\x, point3\y, point1\x, point1\y

SetBuffer BackBuffer()

;Draw the image centered on screen
DrawImage image,400,300


Flip
;Find the scaling factor
sxy# = Input ("What would you like the scaling factor to be? (Ex: 50% = .5)? ==> ") ;What is the scaling factor
Flip

;Scale the image by its scaling factors
ScaleImage image,sxy#,sxy#


;Draw the new image
DrawImage image,400,300

Print "Press any key to exit." 
;Wait for a key before exiting
WaitKey 