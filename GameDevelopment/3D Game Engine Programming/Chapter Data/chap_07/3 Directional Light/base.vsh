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

; texture coords as output
mov oT0, v6
mov oT1, v6         

; (normal) dot (light_to_vector)
; equals lighting intensity per
; vertex
dp3 oD1, v3, -c20