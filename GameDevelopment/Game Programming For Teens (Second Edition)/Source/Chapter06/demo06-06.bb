;demo06-06.bb - A slideshow of images
;Set up graphics
Graphics 800,600

;Set up counter for loading images
snnum = 1


;Grab images from center
AutoMidHandle True


;As long as another image exists, load it and display it
;Since we are loading it to the front buffer, it will be automatically displayed
While (LoadBuffer(FrontBuffer(),"screenshot" + snnum + ".bmp") <>0)
	
	;Write out the name of it
	Text 400,300,"Image screenshot" + snnum + ".bmp"
	
	;Move to next image
	snnum = snnum + 1
	WaitKey
	
	;Flip image on screen
	Flip
	
Wend

Cls
Text 400,300,"Slideshow has ended. Press any key to exit."
WaitKey
End