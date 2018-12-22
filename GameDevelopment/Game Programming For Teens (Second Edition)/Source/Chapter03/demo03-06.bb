;demo03-06.bb - Converts Fahrenheit to Celsius

;MAIN PROGRAM
Print "Welcome to our FtoC converter" 
;get fahrenheit and put it in fvalue
fvalue = Input$("What Fahrenheit value do you wish to convert?")

;Convert fvalue to Celcius
cvalue = ConvertFtoC(fvalue) 

;print results
Print fvalue + " Fahrenheit = " + cvalue + " Celsius." 

;Delay five seconds
Delay 5000

;END OF MAIN PROGRAM

Function ConvertFtoC(fvalue) 
	;convert value and return it
	Return 5.0/9.0 * (fvalue - 32)
End Function 