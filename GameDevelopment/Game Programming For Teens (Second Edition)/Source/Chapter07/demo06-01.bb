;demo06-01.bb - A not-so-greatly animated ship
;Initialize the Graphics
Graphics 800,600,0,2 

SetBuffer FrontBuffer()
;load the ship image
shipimage = LoadImage("ship.bmp") 

;Seed the random generator
SeedRnd(MilliSecs())

;create a ship type
Type ship 
	Field x,y   ;the x and y coords
End Type

;create the ship
ship.ship = New ship 
	
;position the ship randomly
ship\x = Rand(0,800) 
ship\y = Rand(0,600)

While Not KeyDown(1) 
	;Clear the screen
	Cls 
	;move ship left and right
	ship\x = ship\x + Rand(-8,8) 
	;Move ship up And down
	ship\y = ship\y + Rand(-8,8) 
	
	;If ship goes off screen, move it back on
	If ship\x < 0
		ship\x = 15
	ElseIf ship\x > 800
		ship\x = 790
	ElseIf ship\y < 0
		ship\y = 10
	ElseIf ship\y >600
		ship\y = 590
	EndIf

	;Draw the ship
	DrawImage(shipimage,ship\x,ship\y) 
	
Wend