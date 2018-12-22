'**************************************************
'* demo3.bmx									  *
'* © 2005 by N-Software.info, Nicolas De Jaeghere *
'**************************************************

Strict

'** Constants **************************************************************************************
	
	Const WIDTH:Int		= 800
	Const HEIGHT:Int	= 600
	Const DEPTH:Int		= 0
'** End Constants **********************************************************************************

Graphics WIDTH, HEIGHT, DEPTH

AutoMidHandle True

'** Types ******************************************************************************************
	
	Type TPlayer
		
		Const SPEED:Int = 5
		
		Field x:Int
		Field y:Int
		Field image:TImage
	EndType
	
	Type TEnemy Extends TPlayer
		
		Field xd:Int
		Field yd:Int
	EndType
'** End Types **************************************************************************************

'** Global variables *******************************************************************************
	
	Global distance:Int
	Global volume:Float			= .5
	Global bulletSound:TSound	= LoadSound("zing.ogg")
	
	Global player:TPlayer	= New TPlayer
	Global enemy:TEnemy		= New TEnemy
'** End Global variables ***************************************************************************

SeedRnd MilliSecs()

player.x		= WIDTH / 2
player.y		= HEIGHT / 2
player.image	= LoadImage("ship.png")
enemy.x			= WIDTH / 2
enemy.image		= LoadImage("enemyShip.png")
enemy.y			= ImageHeight(enemy.image) / 2 + 10
enemy.xd		= Rnd(-enemy.SPEED, enemy.SPEED)
enemy.yd		= Rnd(-enemy.SPEED, enemy.SPEED)

Local channelBulletSound:TChannel = CueSound(bulletSound)

SetChannelVolume channelBulletSound, volume

'** Main loop **************************************************************************************
	
	Repeat
		
		Cls
		
		testInput()
		draw()
		
		FlushMem
		
		Flip
	Until KeyHit(KEY_ESCAPE)
'** End Main loop **********************************************************************************

End

'** testInput:Int() ********************************************************************************
	
	Function testInput:Int()
		
		If KeyDown(KEY_LEFT) Then
			
			player.x :- player.SPEED
			
			If player.x < ImageWidth(player.image) / 2 Then
				
				player.x = ImageWidth(player.image) / 2
			EndIf
		ElseIf KeyDown(KEY_RIGHT) Then
			
			player.x :+ player.SPEED
			
			If player.x > WIDTH - ImageWidth(player.image) / 2 Then
				
				player.x = WIDTH - ImageWidth(player.image) / 2
			EndIf
		ElseIf KeyDown(KEY_UP) Then
			
			player.y :- player.SPEED
			
			If player.y < ImageHeight(player.image) / 2 Then
				
				player.y = ImageHeight(player.image) / 2
			EndIf
		ElseIf KeyDown(KEY_DOWN) Then
			
			player.y :+ player.SPEED
			
			If player.y > HEIGHT - ImageHeight(player.image) / 2 Then
				
				player.y = HEIGHT - ImageHeight(player.image) / 2
			EndIf
		EndIf
		
		ElseIf KeyHit(KEY_SPACE) Then
			
			findVolume()
			
			Local channelBulletSound:TChannel = CueSound(bulletSound)
			
			SetChannelVolume channelBulletSound, volume
			
			ResumeChannel channelBulletSound
		EndIf
	EndFunction
'** End testInput:Int() ****************************************************************************

'** findVolume:Int() *******************************************************************************
	
	Function findVolume:Int()
		
		distance = findDistance(player.x, player.y, enemy.x, enemy.y)
		
		If distance < 100 Then
			
			volume = 1
		ElseIf distance < 200 Then
			
			volume = .8
		ElseIf distance < 300 Then
			
			volume = .6
		ElseIf distance < 400 Then
			
			volume = .4
		Else
			
			volume = .2
		EndIf
	EndFunction
'** End findVolume:Int() ***************************************************************************

'** findDistance:Int(x1:Int, y1:Int, x2:Int, y2:Int) ***********************************************
	
	Function findDistance:Int(x1:Int, y1:Int, x2:Int, y2:Int)
		
		Return Sqr(((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1)))
	EndFunction
'** End findDistance:Int(x1:Int, y1:Int, x2:Int, y2:Int) *******************************************

'** draw:Int() *************************************************************************************
	
	Function draw:Int()
		
		DrawText "Volume " + volume, 10, HEIGHT - TextHeight("Volume ") - 30
		DrawText "Distance " + distance, 10, HEIGHT - TextHeight("Distance ") - 10
		
		DrawImage enemy.image, enemy.x, enemy.y
		
		updateEnemy()
		
		DrawImage player.image, player.x, player.y
	EndFunction
'** End draw:Int() *********************************************************************************

'** updateEnemy:Int() ******************************************************************************
	
	Function updateEnemy:Int()
		
		enemy.x :+ enemy.xd
		enemy.y :+ enemy.yd
		
		If enemy.x > WIDTH - ImageWidth(enemy.image) / 2 Or enemy.x < ImageWidth(enemy.image) / 2 Then
			
			enemy.xd = -enemy.xd
		ElseIf enemy.y > HEIGHT - ImageHeight(enemy.image) / 2 Or enemy.y < ImageHeight(enemy.image) / 2 Then
			
			enemy.yd = -enemy.yd
		EndIf
	EndFunction
'** End updateEnemy:Int() **************************************************************************
