;demo03-09.bb - Allows user to perform math operations of 1-100

;op1 and op2 are global so they can be accessed from all functions
;op1 contains first operand, op2 contains second
Global op1 
Global op2
Dim array(100) ;0 - 100
InitializeArray()


;continue is 1 as long as program is runnning
continue = 1 


While continue ;as long as the computer wants to play
	;Get the first operand
	op1 = Input("What is the first number? ") 
	;Get the second operand
	op2 = Input("And the second? ") 
	
	; what does the user want to do?
	operator$ = Input("Enter +, -, *, or / ") 
	;Print the answer
	PrintAnswer(operator$)
	
	;Find out if user wants to continue
	continue = Input("Enter 1 to continue or 0 to quit ")

	;Insert a new line
	Print "" 
Wend
End



;This Function sets up the array
Function InitializeArray()
For i=0 To 100
	array(i) = i
Next
End Function



;This function prints the answer to the expression
Function PrintAnswer(operator$)
Print op1 + " " + operator$ + " " + op2 + " is equal to " + FindAnswer(operator$)
End Function	


;This function performs the math based on the user input
Function FindAnswer(operator$)
	Select operator
		Case "+"
			Return array(op1) + array(op2)
		Case "-"
			Return array(op1) - array(op2)
		Case "*"
			Return array(op1) * array(op2)
		Case "/"
			Return array(op1) / array(op2)
	
	End Select
End Function
	