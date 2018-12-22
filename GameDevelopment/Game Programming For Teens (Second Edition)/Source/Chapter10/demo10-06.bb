;demo10-06.bb - Demonstrates the return value of KeyHit()

;Set up graphics so that you can read all of the text, make it windowed
Graphics 800,600,0,2

;Begin introductory text
Text 0,0, "You know what's cool? Game Programming."
Text 0,12,"Although Maneesh ain't that uncool, either."

Flip

;Continue text
Text 0,36, "Anyway, press Esc as many times as you can in the Next 5 seconds."
Text 0,48, "At the end of the program, the number of times will be printed."

Flip

numberofhits=0

;Allow the player 5 seconds to hit esc as many times as possible
timerbegin=MilliSecs()

While timerbegin>MilliSecs()-5000
	If KeyHit(1)
		numberofhits = numberofhits + 1
	EndIf
Wend



;Print the number of times Esc was hit
Text 0,60, "Esc was hit " + numberofhits + " times."
Text 0,72, "You gotta love KeyHit(), huh?"


Flip

;Hold on a sec so the player can see the final text
Delay 5000
