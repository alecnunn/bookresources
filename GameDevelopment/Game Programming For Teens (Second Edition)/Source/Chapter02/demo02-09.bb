;demo02-09.bb - Shows use of the And operator

;find out how old the user is
age = Input$("How old are you? ") 
;find out if the user lives in america
location = Input$("Do you live in America? (1 For yes, 2 For no) ") 

;Write out the proper string depending on the user's age and locations
If age >= 18 And location = 1 Then 
	Print "Congrats, you are eligible to vote!" 
Else
	Print "Sorry, you can't vote."
EndIf

;Wait five seconds
Delay 5000