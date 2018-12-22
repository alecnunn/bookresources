;;;;;;;;;;;;;;;;;;;;;;
;demo05-07.bb
;By Maneesh Sethi
;Demonstrates the Cls function, makes a bunch of colors
;No Input Parameters required
;;;;;;;;;;;;;;;;;;;;;;
Graphics 800,600
SeedRnd (MilliSecs())
msg$ = "Give money to Maneesh: maneesh@maneeshsethi.com"

;Main Loop
While Not KeyDown(1)

;Set the color to a random value
ClsColor Rand(0,255), Rand(0,255), Rand(0,255) 
Cls	  

;If the framecounter goes past 10, draw the msg$ on the screen
If framecount = 10
	Text 200,300,msg$
	framecount = 0
EndIf

framecount = framecount + 1
Delay 100 ;Slow down!
Flip
Wend