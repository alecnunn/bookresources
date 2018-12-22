;--------------------------------------------------------------------
; Standard VertexShader - Part of Liquid Motion
; (c)Copyright 2001 Mystic Game Development
; All rights reserved worldwide
;--------------------------------------------------------------------

; constants
; c60-c63 = world*view*proj matrix
; c64     = halfway vector 1
; c65     = light direction
; c66     = vector to eye
; c67     = light color
; c68     = specular powers

; c70     = material diffuse  color
; c71     = material specular color
; c72     = material ambient  color
; c73     = material emissive color

; vertex data
; v0 = position
; v1 = normal
; v2 = uv coord

; temp registers
; r0 = temp

;--------------------------------------------------------------------

vs.1.1


; transform the final position into clipping space
dp4 oPos.x, v0, c60
dp4 oPos.y, v0, c61
dp4 oPos.z, v0, c62
dp4 oPos.w, v0, c63




;----------------------------
; calculate the vertex color
;----------------------------
; dot normal with light direction in model space
dp3 r0, v1, c65
max r0, r0, c68.x        ; zero out negative values

; multiply diffuse component by material color and store
mul r1, r0.x, c70

; compute SPECULAR part
; dot normal with halfway vector
;dp3 r1, r11, c64
;max r1, r1, c68.x        ; zero out negative values

; raise it to the specular index
;mov r1.w, c68.z
;lit r2, r1

; multiply specular component by material color and 
; add the diffuse color
;mad r1, r2.z, c71, r0

; add ambient
;add r1, r1, c72

; add emissive
;add r1, r1, c73

; multiply light color with material color
;mul oD0, r1, c67

; multiply light color with material color
mul r2, r1, c67



; second light
dp3 r0, v1, c75
max r0, r0, c68.x        ; zero out negative values

; multiply diffuse component by material color and store
mul r0, r0.x, c70
mul r0, r0, c76			; multiply by the light color
add r2, r2, r0




; third light
dp3 r0, v1, c77
max r0, r0, c68.x        ; zero out negative values

; multiply diffuse component by material color and store
mul r0, r0.x, c70
mul r0, r0, c78
add r2, r2, r0

mov oD0, r2










; dot normal with light direction in model space
;dp3 r0, v1, c65
;max r0, r0, c68.x        ; zero out negative values

; multiply diffuse component by material color and store
;mul r0, r0.x, c70

; compute SPECULAR part
; dot normal with halfway vector
;dp3 r1, v1, c64
;max r1, r1, c68.x        ; zero out negative values

; raise it to the specular index
;mov r1.w, c68.z
;lit r2, r1

; multiply specular component by material color and 
; add the diffuse color
;mad r1, r2.z, c71, r0


; add ambient
;add r1, r1, c72

; add emissive
add r1, r1, c73

; multiply light color with material color
;mul oD0, r1, c67





;----------------------------
; texture coord
;----------------------------
mov oT0, v2
