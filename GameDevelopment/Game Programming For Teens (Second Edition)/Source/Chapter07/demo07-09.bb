;demo07-09.bb - Demonstrates Rotation

Graphics 800,600,0,2

;Make sure you handle images from center
AutoMidHandle True

;IMAGES
;Load the image we will be rotating
shipimage = LoadImage ("bullet.bmp")

Print "Welcome to the rotation program."

;Draw the beginning image
DrawImage shipimage,400,300

Flip
;Find out what the rotation value is
rotationvalue# = Input ( "How many degrees would you like to rotate the image? ")
Flip

;Rotate the Image
RotateImage shipimage, rotationvalue#


Print "Your new image is now drawn on the screen"

;Draw the new and rotated image on the screen
DrawImage shipimage, 440,300

Print "Press any key to exit"

;Wait for user to press a key before exiting
WaitKey