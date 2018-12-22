'**************************************************
'* demo11.bmx									  *
'* © 2004 by N-Software.info, Nicolas De Jaeghere *
'**************************************************

Strict

Const STARTSHIPX:Int			= 640 / 2
Const STARTSHIPY:Int			= 480 / 2
Const STARTSHIPHITPOINTS:Int	= 5
Const STARTSHIPSTRING:String	= "<-*->"

Graphics 640, 480, 0

Type ship
	
	Field x:Int
	Field y:Int
	Field hitpoints:Int
	Field shipString:String
End Type

Global running:Int	= 1

Global player:ship	= New ship

player.x			= STARTSHIPX
player.y			= STARTSHIPY
player.hitpoints	= STARTSHIPHITPOINTS
player.shipString	= STARTSHIPSTRING

While running
	
	Cls
	
	testInput()
	draw()
	
	FlushMem
	
	Flip
Wend

Function testInput:Int()
	
	If KeyDown(KEY_LEFT) Then
		
		player.x = player.x - 1
		
		If player.x < 0 Then
			
			player.x = 0
		End If
	End If
	
	If KeyDown(KEY_RIGHT) Then
		
		player.x = player.x + 1
		
		If player.x > 640 Then
			
			player.x = 640
		End If
	End If
	
	If KeyDown(KEY_UP) Then
		
		player.y = player.y - 1
		
		If player.y < 0 Then
			
			player.y = 0
		End If
	End If
	
	If KeyDown(KEY_DOWN) Then
		
		player.y = player.y + 1
		
		If player.y > 480 Then
			
			player.y = 480
		End If
	End If
	
	If KeyHit(KEY_SPACE) Then
		
		player.hitpoints = player.hitpoints - 1
		
		If player.hitpoints <= 0 Then
			
			running = 0
		End If
	End If
	
	If KeyHit(KEY_ESCAPE) Then
		
		running = 0
	End If
End Function

Function draw:Int()
	
	DrawText player.shipString, player.x, player.y
	DrawText "Player X " + player.x, 10, 10
	DrawText "Player y " + player.y, 10, 25
	DrawText "Hitpoints " + player.hitpoints, 10, 40
End Function
