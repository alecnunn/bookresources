;demo03-03.bb.bb - Counts using step amounts
;loop backwards, from 5.0 to 0.0 by -1.2
For counter# = 5.0 To 0.0 Step -1.2 
	;print out 3 digits of counter, so 5.000000 becomes 5.0 - decimal is a digit
	Print Left$( Str counter, 3) 
Next

;Delay for five seconds
Delay 5000