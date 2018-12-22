;demo02-08.bb - tests the keys pressed

x = Input$("Enter 1 to say hi, or 0 to quit. ") 

Select x
	Case 1
		Print "Hi!"
	Case 0
		End
	Default
		Print "Huh?"
End Select

;Wait five seconds
Delay 5000