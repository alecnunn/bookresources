'**************************************************
'* demo10.bmx									  *
'* © 2004 by N-Software.info, Nicolas De Jaeghere *
'**************************************************

'Strict - Goto cannot be used in strict mode

Local inputUser:Int
Local inputUserString:String

#label

Print "Hello World!"

inputUserString = Input()

inputUser = Int(inputUserString)

If inputUser = 1 Then
	
	Goto label
End If

End
