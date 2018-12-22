'**************************************************
'* demo8.bmx									  *
'* © 2004 by N-Software.info, Nicolas De Jaeghere *
'**************************************************

Strict

Local inputUser:Int
Local inputUserString:String

inputUserString = Input("Enter 1 or 2 ")

inputUser = Int(inputUserString)

Select inputUser
	
	Case 1
		
		Print "Input user 1"
	Case 2
		
		End
	Default
		
		Print "?"
End Select

WaitKey

End
