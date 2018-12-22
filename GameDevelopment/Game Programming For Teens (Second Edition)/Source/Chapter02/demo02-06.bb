;demo02-06.bb - Tests if you are old enough to vote

;Ask how old the user is
age = Input$("How old are you? ")

;if older or equal to 18 then let them vote
If age >= 18 Then
	Print "You are legally allowed to vote!" 

;if younger than 18, do not let them vote

Else Print "Sorry, you need To be a few years older." 
EndIf

;Wait five seconds
Delay 5000