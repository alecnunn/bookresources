;demo02-10.bb - Demonstrates the use of Goto
;Place a label
.label 
Print "Hello" 


;Does user want to repeat?
selection = Input("Enter 1 if you want me to repeat 'Hello' ==> ") 
If (selection = 1) 
	
	;Go to the top and print hello again
	Goto label 
EndIf
End