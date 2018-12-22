;demo 07-02.bb - Translates an image using Translate()
Graphics 640,480 

;Set up BackBuffer and AutoMidHandle
SetBuffer BackBuffer() 
AutoMidHandle True

;IMAGES
;Load the ship that will be drawn on the screen
shipimage = LoadImage("enemy.bmp") 

;Make ship start at top left corner
shipx = 0
shipy = 0

While Not KeyDown(1)
;Clear the Screen before every frame
Cls

;Draw the ship at its coordinates
DrawImage shipimage,shipx,shipy


;Translate the ship image 7 pixels right and 5 pixels down
shipx = Translate(shipx,7)
shipy = Translate(shipy,5)

Delay 50
Flip
Wend
;END OF MAIN LOOP

;FUNCTIONS
;Function Translate(x,dx) - Translates a coordinate a specified amount
;x: the coordinate that will be translated
;dx: the translation factor
Function Translate(x,dx)
Return x+dx
End Function