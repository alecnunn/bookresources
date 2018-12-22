; demo06-07.bb - A ReadPixelFast/WritePixeFast Example 

Graphics 350,350,0,2 

Text 0,0, "Press any key to use ReadPixel" 

;Flip text on screen
Flip

;wait for user to do something
WaitKey 

;load rectangle image
image =LoadImage("rectangle.bmp") 

;Draw the intro screen
DrawImage image,0,0 
DrawImage image,100,100

;Flip image on screen
Flip

;Hold up  a second
Delay (1000)

;Create a pixel array that will hold the entire screen
Dim pixelarray(GraphicsWidth(),GraphicsHeight()) 


;lock the buffer REQUIRED
LockBuffer 

;Copy all of the pixels of the screen to the array
For rows=0 To GraphicsWidth()

	For cols=0 To GraphicsHeight() 
	
		;Copy the current pixel
		pixelarray(rows,cols)=ReadPixelFast(rows,cols) 
	Next 
Next 

;Unlock the buffer
UnlockBuffer 

Cls 

Text 0,0, "Press another key to copy pixels backwards"

;Flip text on screen
Flip


;Wait for key press
WaitKey

;Lock the buffer to allow WritePixelFast
LockBuffer 

;Use WritePixelFast to redraw the screen using the color information we got earlier 
For rows=0 To GraphicsWidth() 
	For cols=0 To GraphicsHeight() 
		;Draw the current pixels
		WritePixelFast rows,cols,pixelarray(GraphicsWidth()-rows,cols) 
	Next 
Next 

;Flip image on screen
Flip

; Unlock buffer after using WritePixelFast 
UnlockBuffer 

Text 0,0, "Press a key to exit"

;Flip text on screen
Flip
WaitKey