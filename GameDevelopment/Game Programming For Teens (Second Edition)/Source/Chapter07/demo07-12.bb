;demo 07-12.bb - Demonstrates Tiling
Graphics 800,600

;IMAGES
;Load the image you wish tiled
backgroundimage = LoadImage("stars.bmp")

;Tile the image
TileBlock backgroundimage

;Display
Flip

;Wait for player to press a key before exiting
WaitKey