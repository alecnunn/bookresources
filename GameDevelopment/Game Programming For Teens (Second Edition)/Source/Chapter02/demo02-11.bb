;demo02-11.bb - Try to guess the number
Print "Welcome to the Guessing Game!"
AppTitle "Guessing Game!"
;Seed the random generator...don't worry, it willl be explained later 
SeedRnd MilliSecs() 

;Pick a number between 1 and 100
numbertoguess = Rand(1,100) 

;The num of guesses the user has used
numofguesses = 0

;set the beginning of loop label
.loopbegin 
	;Find out the user's guess
	guess = Input$("Guess a number ") 

	;If player guesses outside of range, tell him to guess again
	If guess > 100 Or guess < 1 
		Print "Pick a number between 1 and 100, silly!"
		;Go back to the beginning
		Goto loopbegin
		
	EndIf
	
	;Add a guess to the guess counter
	numofguesses = numofguesses + 1  

	;If the guess is too low, go back to beginning
	If guess < numbertoguess Then 
		Print "The number was too low."
		Goto loopbegin
	;If guess is too high, go back to the beginning
	Else If guess > numbertoguess Then 
		Print "The number was too high."
		Goto loopbegin
	EndIf


Print "You guessed the number " + numbertoguess + " in " + numofguesses  + " tries!" 

;Wait five seconds
Delay 5000
