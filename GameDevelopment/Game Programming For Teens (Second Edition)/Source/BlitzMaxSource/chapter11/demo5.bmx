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

DrawText "Loading...", WIDTH / 2 - TextWidth("Loading...") / 2, HEIGHT / 2 - TextHeight("Loading...") / 2

Flip

'** Types ******************************************************************************************
	
	Type TBackground
		
		Const SPEED:Int = 1
		
		Field y:Int
		Field image:TImage
	EndType
'** End Types **************************************************************************************

'** Global variables *******************************************************************************
	
	Global volume:Float						= 1
	Global pan:Float						= 0
	Global rate:Float						= 1
	Global humanUpdateSound:TSound			= LoadSound("interimNationHumanUpdate.ogg", True)
	Global channelHumanUpdateSound:TChannel	= CueSound(humanUpdateSound)
	
	Global background:TBackground = New TBackground
'** End Global variables ***************************************************************************

background.y		= 0
background.image	= LoadImage("stars.png")

ResumeChannel channelHumanUpdateSound

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
		
		If KeyHit(KEY_F2) Then
			
			volume :+ .1
			
			If volume > 1 Then
				
				volume = 1
			EndIf
		ElseIf KeyHit(KEY_F1) Then
			
			volume :- .1
			
			If volume < 0 Then
				
				volume = 0
			EndIf
		ElseIf KeyHit(KEY_F6) Then
			
			pan :+ .1
			
			If pan > 1 Then
				
				pan = 1
			EndIf
		ElseIf KeyHit(KEY_F5) Then
			
			pan :- .1
			
			If pan < -1 Then
				
				pan = -1
			EndIf
		ElseIf KeyHit(KEY_F10) Then
			
			rate :+ .1
			
			If rate > 5 Then
				
				rate = 5
			EndIf
		ElseIf KeyHit(KEY_F9) Then
			
			rate :- .1
			
			If rate < 0 Then
				
				rate = 0
			EndIf
		ElseIf KeyHit(KEY_S) Then
			
			StopChannel channelHumanUpdateSound
		ElseIf KeyHit(KEY_P) Then
			
			PauseChannel channelHumanUpdateSound
		ElseIf KeyHit(KEY_R) Then
			
			ResumeChannel channelHumanUpdateSound
		EndIf
		
		SetChannelVolume channelHumanUpdateSound, volume
		
		SetChannelPan channelHumanUpdateSound, pan
		
		SetChannelRate channelHumanUpdateSound, rate
	EndFunction
'** End testInput:Int() ****************************************************************************

'** draw:Int() *************************************************************************************
	
	Function draw:Int()
		
		TileImage background.image, 0, background.y
		
		updateBackground()
		
		DrawText "Volume " + volume, 10, HEIGHT - TextHeight("Volume ") - 50
		DrawText "Pan " + pan, 10, HEIGHT - TextHeight("Pan ") - 30
		DrawText "Rate " + rate, 10, HEIGHT - TextHeight("Rate ") - 10
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
