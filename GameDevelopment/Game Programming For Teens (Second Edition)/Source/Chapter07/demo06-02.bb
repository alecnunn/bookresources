;demo06-02.bb - Draws a bunch of random lines to screen.

;Initialize Graphics
Graphics 800,600,0,2


;Draw only to the front buffer
SeedRnd(MilliSecs()) 

While (Not KeyDown(1)) 
	;Set a random color
	Color Rand(0,255),Rand(0,255),Rand(0,255) 

	;Draw a random line
	Line Rand(0,800), Rand(0,600), Rand(0,800), Rand(0,600) 

	;Slow it down
	Delay(25) 
	Flip
Wend