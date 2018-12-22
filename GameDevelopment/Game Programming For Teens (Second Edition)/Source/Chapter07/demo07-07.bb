;demo07-07.bb - Demonstrates the use of ScaleImage
Graphics 800,600,0,2

;Set automidhandle to true
AutoMidHandle True

;IMAGES
;Load the spaceship that will be drawn on screen
spaceshipimage = LoadImage("spaceship.bmp")

;Draw the space ship directly in the center of the screen
DrawImage spaceshipimage, 400,300

Flip
;Find out what the player wants the x and y scaling factors to be
xs# = Input("What would you like the x scaling value to be? ")
ys# = Input("What would you like the y scaling value to be? ")
Flip

;Prepare the screen for the scaled spaceship by clearing it
Cls
;Scale the image
ScaleImage spaceshipimage, xs#,ys#

;Draw the new scaled spaceship
DrawImage spaceshipimage, 400,300

Print "This is your updated image"
Print "Press any key to exit"

;Wait for user to press a key before exiting
WaitKey