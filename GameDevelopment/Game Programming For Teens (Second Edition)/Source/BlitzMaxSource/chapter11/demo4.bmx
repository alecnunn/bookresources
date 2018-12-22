'**************************************************
'* demo4.bmx									  *
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
	
	Type TEnemy Extends TBackground
		
		Const SPEED:Int = 5
		
		Field x:Int
		Field xd:Int
	EndType
'** End Types **************************************************************************************

'** Global variables *******************************************************************************
	
	Global pan:Int				= 0
	Global playerImage:TImage	= LoadImage("ship.png")
	Global bulletSound:TSound	= LoadSound("zing.ogg")
	
	Global background:TBackground	= New TBackground
	Global enemy:TEnemy				= New TEnemy
'** End Global variables ***************************************************************************

SeedRnd MilliSecs()

background.y		= 0
background.image	= LoadImage("stars.png")
enemy.x				= WIDTH / 2
enemy.y				= HEIGHT / 2
enemy.xd			= Rnd(-enemy.SPEED, enemy.SPEED)
enemy.image			= LoadImage("enemyShip.png")

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
		
		If KeyHit(KEY_SPACE) Then
			
			pan = findPan()
			
			Local channelBulletSound:TChannel = CueSound(bulletSound)
			
			SetChannelPan channelBulletSound, pan
			
			ResumeChannel channelBulletSound
		EndIf
	EndFunction
'** End testInput:Int() ****************************************************************************

'** findPan:Int() **********************************************************************************
	
	Function findPan:Int()
		
		If MouseX() > enemy.x Then
			
			Return 1
		ElseIf MouseY() < enemy.x Then
			
			Return -1
		Else
			
			Return 0
		EndIf
	EndFunction
'** End findPan:Int() ******************************************************************************

'** draw:Int() *************************************************************************************
	
	Function draw:Int()
		
		DrawText "Pan " + pan, 10, HEIGHT - TextHeight("Pan ") - 10
		
		TileImage background.image, 0, background.y
		
		updateBackground()
		
		DrawImage playerImage, MouseX(), MouseY()
		DrawImage enemy.image, enemy.x, enemy.y
		
		updateEnemy()
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

'** updateEnemy:Int() ******************************************************************************
	
	Function updateEnemy:Int()
		
		enemy.x :+ enemy.xd
		
		If enemy.x > WIDTH - ImageWidth(enemy.image) / 2 Or enemy.x < ImageWidth(enemy.image) / 2 Then
			
			enemy.xd = -enemy.xd
		EndIf
	EndFunction
'** End updateEnemy:Int() **************************************************************************
