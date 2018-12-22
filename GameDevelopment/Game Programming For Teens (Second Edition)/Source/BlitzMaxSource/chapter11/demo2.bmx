'**************************************************
'* demo2.bmx									  *
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

'** Global variables *******************************************************************************
	
	Global rate:Int					= 1
	Global playerImage:TImage		= LoadImage("ship.png")
	Global explosionSound:TSound	= LoadSound("explode.ogg")
'** End Global variables ***************************************************************************

'** Main loop **************************************************************************************
	
	Repeat
		
		Cls
		
		If KeyHit(KEY_UP) Then
			
			rate :+ 1
			
			If rate > 10 Then
				
				rate = 10
			EndIf
		ElseIf KeyHit(KEY_DOWN) Then
			
			rate :- 1
			
			If rate < 0 Then
				
				rate = 0
			EndIf
		ElseIf KeyHit(KEY_SPACE) Then
			
			Local channelExplosionSound:TChannel = CueSound(explosionSound)
			
			SetChannelRate channelExplosionSound, rate
			
			ResumeChannel channelExplosionSound
		EndIf
		
		DrawText "Rate " + rate, 10, HEIGHT - TextHeight("Rate") - 10
		
		DrawImage playerImage, MouseX(), MouseY()
		
		FlushMem
		
		Flip
	Until KeyHit(KEY_ESCAPE)
'** End Main loop **********************************************************************************

End
