;demo06-05.bb - Demonstrates the usage of SaveBuffer()
Graphics 800,600

;Seed the Random Generator
SeedRnd (MilliSecs()) 

;68 is keycode for f10
Const f10key = 68 

;Screenshot number, begin at one (it is an integer)
snnum = "1" 

While Not KeyDown(1)
	
	;Set up random color
	Color Rand(0,255),Rand(0,255),Rand(0,255)
	
	;Draw a random rectangle
	Rect Rand(0,800),Rand(0,600),Rand(0,200),Rand(0,200),Rand(0,1)

	;Wait a little while
	Delay 45
	
	;If user presses f10, take a screenshot
	If KeyHit(f10key)
		SaveBuffer(FrontBuffer(), "screenshot" + snnum + ".bmp") 
		
		snnum = snnum + 1 ;Add 1 to the end of the filename
	EndIf
	
	;Flip image on screen
	Flip
Wend