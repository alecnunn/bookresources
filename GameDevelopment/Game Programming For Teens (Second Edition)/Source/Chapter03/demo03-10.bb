;demo03-10.bb - Draws out 25 '*'s and 25 '+'s

;create the array
Dim starsplusses$(2,24) 

;initialize the array. The first dimension will contain *'s and the second will contain +'s
For rows = 0 To 1 
	For columns=0 To 24
		;Assign either + or *, depending on the return value of FindChar$()
		starsplusses$(rows,columns) = FindChar$(rows)
	Next
	
Next

;display the array
For rows = 0 To 1 
	For columns = 0 To 24
		;Write each value to the screen
		Write starsplusses$(rows,columns)
	Next
	;write a new line after each row
	Print "" 
Next

;Delay five seconds
Delay 5000

;FUNCTION FINDCHAR$(i)
;returns * or +
Function FindChar$(i)
	If i = 0
		Return "*"
	Else If i = 1 
		Return "+"
	EndIf
End Function