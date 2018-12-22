'**************************************************
'* demo11.bmx									  *
'* © 2004 by N-Software.info, Nicolas De Jaeghere *
'**************************************************

'Strict - Goto cannot be used in strict mode

SeedRnd MilliSecs()

Local guessNumber:Int
Local guessNumberString:String
Local numberToGuess:Int		= Rand(1, 100)
Local numberOfGuesses:Int	= 0

#begin
	
	guessNumberString = Input("Guess number ")
	
	guessNumber = Int(guessNumberString)
	
	If guessNumber > 100 Or guessNumber < 1 Then
		
		Print "Guess number > 100 or Guess number < 1."
		
		Goto begin
	End If
	
	numberOfGuesses = numberOfGuesses + 1
	
	If guessNumber > numberToGuess Then
		
		Print "Guess number > Number to guess"
		
		Goto begin
	Else If guessNumber < numberToGuess Then
		
		Print "Guess number < Number to guess"
		
		Goto begin
	End If
	
	Print "Number to guess " + numberToGuess
	Print "Number of guesses " + numberOfGuesses
	
	WaitKey
	
	End
	