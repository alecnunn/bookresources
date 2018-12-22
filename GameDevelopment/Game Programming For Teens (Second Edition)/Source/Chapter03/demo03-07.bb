;demo03-07.bb - Fixed CallMe() 
Global x
;call CallMe() function
CallMe() 

;concatenate "x is equal to" and the variable x
Print "x is equal to " + x 

;Delay five seconds
Delay 5000

;FUNCTION CALLME()
;sets x to 314
Function CallMe()
	x = 314
End Function