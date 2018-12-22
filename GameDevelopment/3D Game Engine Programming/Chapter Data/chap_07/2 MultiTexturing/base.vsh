vs.1.1                 
dcl_position  v0       
dcl_normal    v3       
dcl_texcoord  v6       
dcl_tangent   v8

; transform position
m4x4 oPos, v0, c0   

; ambient light as diffuse 
; color output
mov oD0, c4

; texture coords for each
; stage we use, i.e. 0 and 1
mov oT0, v6           
mov oT1, v6

   