;;;;;;;;;;;;;;;;;;
;demo06-04.bb
;By Maneesh Sethi
;Demonstrates CreateImage
;No Input Parameters Required
;;;;;;;;;;;;;;;;;;

;Set up graphics mode
Graphics 800,600 

;Set automidhandle to true
AutoMidHandle True

;create the blank image
wallimg = CreateImage(200,200) 

;set the buffer to the image
SetBuffer ImageBuffer(wallimg) 

;make the color white
Color 255,255,255 

;draw a rectangle from topleft to bottomright of buffer
Rect 0,0,200,200 

;switch back to back buffer
SetBuffer BackBuffer() 

;Flip the image on screen
Flip

;draw the image buffer
DrawImage wallimg,400,300 

;Flip on screen
Flip

;wait for user To press a key Before exiting
WaitKey 