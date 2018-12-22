;demo02-05.bb - Tests if you are old enough to vote

;Find out how old the user is
age = Input$("How old are you? ") 
;if older or equal to 18, print out confirmation that user is allowed to vote.
If age >= 18 Then 
	Print "You are legally allowed to vote!"
EndIf
;Wait five seconds
Delay 5000