'**************************************************
'* demo2.bmx									  *
'* © 2005 by N-Software.info, Nicolas De Jaeghere *
'**************************************************

Strict

'** Constants **************************************************************************************
	
	Const WIDTH:Int		= 800
	Const HEIGHT:Int	= 600
	Const DEPTH:Int		= 0
	Const OBJECTS:Int	= 100
'** End Constants **********************************************************************************

Graphics WIDTH, HEIGHT, DEPTH

AutoMidHandle True

'** Types ******************************************************************************************
	
	Type TPoint
		
		Field x:Int
		Field y:Int
	EndType
	
	Type TPlayer
		
		Const SPEED:Int = 5
		
		Field x:Int
		Field y:Int
		Field radius:Int
		Field collisions:Int
		Field image:TImage
	EndType
'** End Types **************************************************************************************

'** Global variables *******************************************************************************
	
	Global player:TPlayer = New TPlayer
	
	Global pointList:TList = CreateList()
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
		
		player.collisions	= 0
		player.image		= LoadImage("ship.png")
		player.radius 		= findRadius(player.image)
		
		Local counter:Int
		
		For counter = 1 To OBJECTS
			
			ListAddLast pointList, New TPoint
		Next
		
		newLevel()
	EndFunction
'** End newGame:Int() ******************************************************************************

'** findRadius:Int(image:TImage) *************************************************************
	
	Function findRadius:Int(image:TImage)
		
		Return (ImageWidth(image) / 2 + ImageHeight(image) / 2) / 2
	EndFunction
'** End findRadius:Int(image:TImage) *********************************************************

'** newLevel:Int() *********************************************************************************
	
	Function newLevel:Int()
		
		player.x	= WIDTH / 2
		player.y	= HEIGHT / 2
		
		Local counter:TPoint = New TPoint
		
		For counter = EachIn pointList
			
			counter.x	= Rnd(WIDTH)
			counter.y	= Rnd(HEIGHT)
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
	EndFunction
'** End testInput:Int() ****************************************************************************

'** draw:Int() *************************************************************************************
	
	Function draw:Int()
		
		DrawImage player.image, player.x, player.y
		
		Local counter:TPoint = New TPoint
		
		For counter = EachIn pointList
			
			Plot counter.x, counter.y
		Next
		
		DrawText "X " + player.x, 10, HEIGHT - TextHeight("X ") - 50
		DrawText "Y " + player.y, 10, HEIGHT - TextHeight("Y ") - 30
		DrawText "Collisoins " + player.collisions, 10, HEIGHT - TextHeight("Collisions ") - 10
		
		checkPlayer()
	EndFunction
'** End draw:Int() *********************************************************************************

'** checkPlayer:Int() ******************************************************************************
	
	Function checkPlayer:Int()
		
		Local counter:TPoint = New TPoint
		
		For counter = EachIn pointList
			
			If findDistance(player.x, player.y, counter.x, counter.y) < player.radius Then
				
				player.collisions :+ 1
				
				newLevel()
			EndIf
		Next
	EndFunction
'** End checkPlayer:Int() **************************************************************************

'** findDistance:Int(x1:Int, y1:Int, x2:Int, y2:Int) ***********************************************
	
	Function findDistance:Int(x1:Int, y1:Int, x2:Int, y2:Int)
		
		Return Sqr((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1))
	EndFunction
'** End findDistance:Int(x1:Int, y1:Int, x2:Int, y2:Int) *******************************************
