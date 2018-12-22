'**************************************************
'* demo1.bmx									  *
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
	
	Type TBackground
		
		Const SPEED:Int = 1
		
		Field y:Int
		Field image:TImage
	EndType
	
	Type TPlayer Extends TBackground
		
		Const SPEED:Int = 5
		
		Field x:Int
	EndType
	
	Type TEnemy Extends TPlayer
		
		Field xd:Int
		Field yd:Int
	EndType
	
	Type TBullet Extends TPlayer
		
		Const SPEED:Int = 7
	EndType
'** End Types **************************************************************************************

'** Global variables *******************************************************************************
	
	Global bulletImage:TImage		= LoadImage("bullet.png")
	Global bulletSound:TSound		= LoadSound("zing.ogg")
	Global explosionSound:TSound	= LoadSound("explode.ogg")
	
	Global player:TPlayer			= New TPlayer
	Global enemy:TEnemy				= New TEnemy
	Global background:TBackground	= New TBackground
	Global bulletList:TList			= New TList
'** End Global variables ***************************************************************************

SeedRnd MilliSecs()

newGame()

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

'** newGame:Int() **********************************************************************************
	
	Function newGame:Int()
		
		player.image		= LoadImage("ship.png")
		enemy.image			= LoadImage("enemyShip.png")
		background.image	= LoadImage("stars.png")
		
		newLevel()
	EndFunction
'** End newGame:Int() ******************************************************************************

'** newLevel:Int() *********************************************************************************
	
	Function newLevel:Int()
		
		player.x		= WIDTH / 2
		player.y		= HEIGHT / 2
		enemy.x			= WIDTH / 2
		enemy.y			= ImageHeight(enemy.image) / 2 + 10
		enemy.xd		= Rnd(-enemy.SPEED, enemy.SPEED)
		enemy.yd		= Rnd(-enemy.SPEED, enemy.SPEED)
		background.y	= 0
		
		Local counter:TBullet = New TBullet
		
		For counter = EachIn bulletList
			
			ListRemove bulletList, counter
		Next
	EndFunction
'** End newLevel:Int() *****************************************************************************

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
		
		If KeyHit(KEY_SPACE) Then
			
			Local bullet:TBullet = New TBullet
			
			bullet.x		= player.x
			bullet.y		= player.y
			bullet.image	= bulletImage
			
			PlaySound bulletSound
			
			ListAddLast bulletList, bullet
		EndIf
	EndFunction
'** End testInput:Int() ****************************************************************************

'** draw:Int() *************************************************************************************
	
	Function draw:Int()
		
		TileImage background.image, 0, background.y
		
		updateBackground()
		updateBullets()
		updateEnemy()
		
		DrawImage player.image, player.x, player.y
		DrawImage enemy.image, enemy.x, enemy.y
	EndFunction
'** End draw:Int() *********************************************************************************

'** updateBackground:Int() *************************************************************************
	
	Function updateBackground:Int()
		
		background.y :+ background.SPEED
		
		If background.y >= ImageHeight(background.image) Then
			
			background.y = 0
		EndIf
	EndFunction
'** End updateBackground:Int() *********************************************************************

'** updateBullets:Int() ****************************************************************************
	
	Function updateBullets:Int()
		
		Local counter:TBullet = New TBullet
		
		For counter = EachIn bulletList
			
			counter.y :- counter.SPEED
			
			If ImagesCollide(counter.image, counter.x, counter.y, 0, enemy.image, enemy.x, enemy.y, 0) Then
				
				PlaySound explosionSound
				
				Cls
				
				DrawText "You destroyed the enemy", WIDTH / 2 - TextWidth("You destroyed the enemy") / 2, HEIGHT / 2 - TextHeight("You destroyed the enemy") / 2
				
				Flip
				
				newLevel()
				
				Delay 5000
				
				Return
			EndIf
			
			If counter.y < -ImageHeight(counter.image) / 2 Then
				
				ListRemove bulletList, counter
			Else
				
				DrawImage counter.image, counter.x, counter.y
			EndIf
		Next
	EndFunction
'** End updateBullets:Int() ************************************************************************

'** updateEnemy:Int() ******************************************************************************
	
	Function updateEnemy:Int()
		
		enemy.x :+ enemy.xd
		enemy.y :+ enemy.yd
		
		If enemy.x > WIDTH - ImageWidth(enemy.image) / 2 Or enemy.x < ImageWidth(enemy.image) / 2 Then
			
			enemy.xd = -enemy.xd
		ElseIf enemy.y > HEIGHT - ImageWidth(enemy.image) / 2 Or enemy.y < ImageHeight(enemy.image) / 2 Then
			
			enemy.yd = -enemy.yd
		EndIf
	EndFunction
'** End updateEnemy:Int() **************************************************************************
