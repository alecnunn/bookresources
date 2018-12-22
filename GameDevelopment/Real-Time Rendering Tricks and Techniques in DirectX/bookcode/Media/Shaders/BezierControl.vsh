vs.1.1

;Do the final transformation
dp4 oPos.x, v0, c0
dp4 oPos.y, v0, c1
dp4 oPos.z, v0, c2
dp4 oPos.w, v0, c3

;Output the color stored in c4. This allows the 
;application to set the vertex color dynamically.
mov oD0, c4


