'**************************************************
'* demo9.bmx									  *
'* © 2004 by N-Software.info, Nicolas De Jaeghere *
'**************************************************

Strict

Local age:Int
Local location:Int
Local ageString:String
Local locationString:String

ageString		= Input("Age ")
locationString	= Input("Location ")

age			= Int(ageString)
location	= Int(locationString)

If age >= 18 And location = 1 Then
	
	Print "Age >= 18 and location = 1"
Else
	
	Print "Age is not >= 18 and/or location is not = 1"
End If

WaitKey

End
