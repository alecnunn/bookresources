;demo03-05.bb  - Closes program after player presses ESC.
Graphics 640,480
Text 0,0, "Why did you open this program?"
Flip
;y is the variable that judges the location of the text
y=12
Repeat 
	;Print text
	Text 0,y, "Press Esc to exit."
	;wait a sec
	Delay 1000 
	Flip
	
	;Move next line of text down
	y=y+12
	
;repeat until user hits esc
Until KeyHit(1) 
Text 0,y, "Program is ending."
Flip