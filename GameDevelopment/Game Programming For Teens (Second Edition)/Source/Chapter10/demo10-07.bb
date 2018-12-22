;demo10-07.bb - What if there is no mouse cursor?
Graphics 640,480
;Set default drawing surface to back buffer
SetBuffer BackBuffer()

;MAIN LOOP
While Not KeyDown(1)

;Clear the Screen
Cls
;Print out text
Text 0,0,"Try guessing where the mouse actually is!"

;Print X and Y coordinates
Text 0,12, "MouseX: " + MouseX()
Text 0,24, "MouseY: " + MouseY()

;Flip front and back buffers
Flip

Wend ;End of Main loop