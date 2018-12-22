;demo03-08.bb - initializes 10 vars to 314

;Declare array
Dim variable(10) 

For i=0 To 10 

	;set array to 314
	variable(i) = 314 
	Print variable(i)
Next

;Delay five seconds
Delay 5000
