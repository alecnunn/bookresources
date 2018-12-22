;;;;;;;;;;;;;;;;;;;;;;
;demo05-06.bb
;By Maneesh Sethi
;Demonstrates the Color function, draws ellipses
;No Input Parameters required
;;;;;;;;;;;;;;;;;;;;;;
Graphics 800,600

;Seed random generator
SeedRnd (MilliSecs())


;Max width of ellipse
Const MAXWIDTH = 200 
 ;Max Height of ellipse
Const MAXHEIGHT = 200

;Main Loop
While Not KeyDown(1) 

;Clear the Screen
Cls

;Set the color to a random value
Color Rand(0,255), Rand(0,255), Rand(0,255) 


;Draw a random oval
Oval Rand(0,800),Rand(0,600),Rand(0,MAXWIDTH),Rand(0,MAXHEIGHT), Rand(0,1) 

;Slow down!
Delay 50 
Flip
Wend