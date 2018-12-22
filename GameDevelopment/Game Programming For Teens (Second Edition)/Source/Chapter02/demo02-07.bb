;demo02-07.bb - Tests if you are old enough to vote

;find out how old the user is
age = Input$("How old are you? ") 


;if exactly 18, write that voting is legal
If age = 18 Then 
	Print "You can now vote."	
;if older tha 18, write out that voting has been legal for a while
Else If age > 18 
	Print "You've been able to vote for a while."
;if younger than 18, write out that voting is illegal.
Else If age < 18 
	Print "Sorry, you will have to wait a few years to vote."
EndIf

;Wait five seconds
Delay 5000