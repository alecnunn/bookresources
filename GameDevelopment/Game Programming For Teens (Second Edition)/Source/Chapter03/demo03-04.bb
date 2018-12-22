;demo03-04.bb - Waits for a key and then exits
Graphics 640,480
Text 0,0, "This program is worthless."
Text 0,12,"Press escape to exit."
Flip
;Wait until user presses 1 to Escape
While Not KeyDown(1) 
Wend 
End