'**************************************************
'* demo9.bmx									  *
'* © 2004 by N-Software.info, Nicolas De Jaeghere *
'**************************************************

Strict

Global array:Int[100]

Local number1:Int
Local number2:Int
Local runningString:String
Local operator:String
Local number1String:String
Local number2String:String
Local running:Int = 1

initializeArray()

While running
	
	number1String	= Input("Number 1 ")
	number2String	= Input("Number 2 ")
	operator		= Input("Operator (+, -, *, /) ")
	
	number1 = Int(number1String)
	number2 = Int(number2String)
	
	printAnswer(number1, number2, operator)
	
	runningString = Input("Quit? (Enter 1 or 2) ")
	
	running = Int(runningString) - 1
	
	Print ""
Wend

End

Function initializeArray:Int()
	
	Local i:Int
	
	For i = 0 To 99
		
		array[i] = i
	Next
End Function

Function printAnswer:Int(number1:Int, number2:Int, operator:String)
	
	Print number1 + " " + operator + " " + number2 + " = " + answer(number1, number2, operator)
End Function

Function answer:Int(number1:Int, number2:Int, operator:String)
	
	Select operator
		
		Case "+"
			
			Return number1 + number2
		Case "-"
			
			Return number1 - number2
		Case "*"
			
			Return number1 * number2
		Case "/"
			
			Return number1 / number2
	End Select
End Function
