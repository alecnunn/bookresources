'**************************************************
'* demo5.bmx									  *
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
	
	Type TBullet Extends TPlayer
		
		Const SPEED:Int = 7
	EndType
'** End Types **************************************************************************************

'** Global variables *******************************************************************************
	
	Global bulletImage:TImage = LoadImage("bullet.png")
	
	Global player:TPlayer			= New TPlayer
	Global background:TBackground	= New TBackground
	Global bulletList:TList			= New TList
'** End Global variables ***************************************************************************

player.x			= WIDTH / 2
player.y			= HEIGHT / 2
player.image		= LoadImage("ship.png")
background.y		= 0
background.image	= LoadImage("stars.png")

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
		
		If KeyHit(KEY_SPACE) Then
			
			Local bullet:TBullet = New TBullet
			
			bullet.x		= player.x
			bullet.y		= player.y
			bullet.image	= bulletImage
			
			ListAddLast bulletList, bullet
		EndIf
	EndFunction
'** End testInput:Int() ****************************************************************************

'** draw:Int() *************************************************************************************
	
	Function draw:Int()
		
		TileImage background.image, 0, background.y
		
		updateBackground()
		updateBullets()
		
		DrawImage player.image, player.x, player.y
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
			
			If counter.y < -ImageHeight(counter.image) / 2 Then
				
				ListRemove bulletList, counter
			Else
				
				DrawImage counter.image, counter.x, counter.y
			EndIf
		Next
	EndFunction
'** End updateBullets:Int() ************************************************************************
