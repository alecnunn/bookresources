;;;;;;;;;;;;;;;;;;;;;;
;demo05-04.bb
;By Maneesh Sethi
;Demonstrates the problems with masking
;No Input Parameters required
;;;;;;;;;;;;;;;;;;;;;;
;Initialize graphics
Graphics 640,480


;Load Background
lilliesimage = LoadImage("lillies.bmp") 
;Draw the background
DrawImage lilliesimage,0,0 

;Load the frog
frogimage = LoadImage("frog.bmp")

;Center the frog
MidHandle frogimage

;Draw it in the center
DrawImage frogimage,320,240

Flip

;Wait for user to press a button
WaitKey