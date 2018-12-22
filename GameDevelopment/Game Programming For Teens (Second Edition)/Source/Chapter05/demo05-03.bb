;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; demo05-03.bb;;;;;;;;;;;;;;;;;;;;;
; By Maneesh Sethi;;;;;;;;;;;;;;;;;
; Creates an image and displays it!
; No input parameters required;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;INITIALIZATION

;Set up the graphics
Graphics 800,600


;Seed the Random Generator
SeedRnd MilliSecs()

;CONSTANTS
;The length of each block
Const LENGTH = 100

;The height of each block
Const HEIGHT = 100

;The amount of dots in each block
Const DOTS = 100
;END CONSTANTS



;IMAGES
;Create the dotfield image
dotfieldimage = CreateImage(LENGTH,HEIGHT)
;END IMAGES

;For each dot, draw a random dot at a random location
For loop = 0 To DOTS ;For every star
;draw only on created image
SetBuffer ImageBuffer(dotfieldimage)

;Plot the dot
Plot Rnd(LENGTH), Rnd(HEIGHT)

Next

;Set buffer back to normal
SetBuffer BackBuffer()
;END INITIALIZATION

;MAIN LOOP

;Tile the image until the user quits (presses ESC)

Cls
TileImage dotfieldimage
Flip

WaitKey


;END MAIN LOOP