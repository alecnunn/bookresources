;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;demo06-08.bb;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;By Maneesh Sethi;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;This program allows the user to draw a picture.;;;;;
;There are no input variables required;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;INITIALIZATION
;Set up Windowed Graphics Mode
Graphics 640,480,0,2 
;Backbuffer
SetBuffer BackBuffer() 


;Constant the keyboard keys
Const ESCKEY = 1, ONEKEY = 2,TWOKEY = 3, THREEKEY = 4, FOURKEY = 5, FIVEKEY = 6, F10KEY = 68 

;Tweak these numbers
Const BUFFERHEIGHT = 480, BUFFERWIDTH = 640 

;GLOBAL IMAGES
Global greenimage = LoadImage("greencolor.bmp")
Global redimage = LoadImage("redcolor.bmp")
Global blueimage = LoadImage("bluecolor.bmp")
Global blackimage = LoadImage("blackcolor.bmp")
Global whiteimage = LoadImage("whitecolor.bmp")
Global mouseimage = LoadImage("mousearrow.bmp")
; Create a blank image that will be used to draw on
Global picturebuffer = CreateImage(BUFFERWIDTH,BUFFERHEIGHT) 
;VARIABLES
;Automatically select green as the color
Global selectedcolor = 1 

;MASKS
;Mask the white around icon
MaskImage mouseimage,255,255,255 
;change mask so black is visible
MaskImage blackimage,255,255,255 
;END INITIALIZATION

;MAIN LOOP
While Not KeyDown(ESCKEY)

;Clears the screen
Cls 
;Draws everything text related
DrawAllText() 

;Draws the mouse cursor
DrawMouse() 

;Test what keyboard buttons were pressed
TestKeyboardInput() 

;Test to see if user pressed any mouse buttons
TestMouseInput() 

;draw the picture
DrawImage picturebuffer,0,100 

;flip the buffers
Flip 
Wend 
;END MAIN LOOP
;EXIT
;
;END EXIT

;FUNCTIONS

;;;;;;;;;;;;;;;;;;
;Function DrawAllText()
;Calls functions that draw HUD of program
;No Parameters
;;;;;;;;;;;;;;;;;;

Function DrawAllText()

;Draws the color choices
DrawTextInfo() 
;Draws the selected color
ColorText() 

;Draws the location of the text
MouseText() 
End Function


;;;;;;;;;;;;;;;;;;
;Function TestMouseInput()
;If player presses on mouse, draw the color
;No Parameters
;;;;;;;;;;;;;;;;;;
Function TestMouseInput()


;If player presses the left mouse button, draw the selected color
If MouseDown(1) 
	

	;Begin drawing only on image
	SetBuffer(ImageBuffer(picturebuffer)) 

	; draw the selected color at the mouse location
	Select (selectedcolor) 
	Case 1
		DrawImage(greenimage,MouseX(),MouseY()-100)
	Case 2
		DrawImage(redimage,MouseX(),MouseY()-100)
	Case 3
		DrawImage(blueimage,MouseX(),MouseY()-100)

	Case 4
		DrawImage(blackimage,MouseX(),MouseY()-100)

	Case 5	
		DrawImage(whiteimage,MouseX(),MouseY()-100)

End Select
End If

;reset the buffer back to the back buffer
SetBuffer BackBuffer() 

End Function


;;;;;;;;;;;;;;;;;;
;Function TestKeyboardInput()
;Tests if the keyboard wants to change the selected color or take a screenshot
;No Parameters
;;;;;;;;;;;;;;;;;;


Function TestKeyboardInput()

;If user presses a number, select the corresponding color
If KeyDown(ONEKEY)
	selectedcolor = 1
ElseIf KeyDown(TWOKEY)
	selectedcolor = 2
ElseIf KeyDown(THREEKEY)
	selectedcolor = 3
ElseIf KeyDown(FOURKEY)
	selectedcolor = 4
ElseIf KeyDown(FIVEKEY)
	selectedcolor = 5
EndIf


;If user presses F10, take a screenshot
If KeyDown(F10KEY) 
	;Save the picture buffer as screenshot.bmp
	SaveBuffer ImageBuffer(picturebuffer), "screenshot.bmp" 
EndIf

End Function 


;;;;;;;;;;;;;;;;;;
;Function DrawMouse()
;Draws the Mouse Cursor
;No Parameters
;;;;;;;;;;;;;;;;;;
Function DrawMouse()

;Draw the mouse at the mouses location
DrawImage mouseimage,MouseX(),MouseY() 
End Function


;;;;;;;;;;;;;;;;;;
;Function ColorText()
;Chooses the selected color and writes it on the scren
;No Parameters
;;;;;;;;;;;;;;;;;;
Function ColorText()

;Assign the name of the color to selectedcolortext$
Select (selectedcolor) 
	Case 1
			selectedcolortext$ = "Green"
	Case 2
			selectedcolortext$ = "Red"
	Case 3
			selectedcolortext$ = "Blue"
	Case 4
			selectedcolortext$ = "Black"
	Case 5
			selectedcolortext$ = "White"
End Select

;Write out the selected color
Text 240, 20, "Selected Color: " + selectedcolortext$ 

End Function

;;;;;;;;;;;;;;;;;;
;Function MouseText()
;Writes the mouses location on the screen
;No Parameters
;;;;;;;;;;;;;;;;;;

Function MouseText()
mousextext$ = "Mouse X: " + MouseX()
mouseytext$ = "Mouse Y: " + MouseY()
Text 540,20,mousextext$
Text 540,40,mouseytext$
End Function

;;;;;;;;;;;;;;;;;;
;Function DrawTextInfo()
;Displays the user's color choices
;No Parameters
;;;;;;;;;;;;;;;;;;
Function DrawTextInfo()
;Display color choice
Text 0,0, "Press the number of the color you wish to draw"
Text 0,12, "Colors:"
Text 0,24, "1. Green"
Text 0,36,"2. Red"
Text 0,48,"3. Blue"
Text 0,60,"4. Black"
Text 0,72,"5. White"
Text 0,84,"Press F10 to save image (text WILL NOT be saved)"
End Function

;END FUNCTIONS