;--------------------------------------------------------------------
; SoftSkinning VertexShader - Part of Liquid Motion
; (c)Copyright 2001 Mystic Game Development
; All rights reserved worldwide
;--------------------------------------------------------------------

; constants
; c0-c59  = bone matrices
; c60-c63 = world/view/proj matrix
; c89     = material color

; c70     = material diffuse  color
; c71     = material specular color
; c72     = material ambient  color
; c73     = material emissive color

; vertex data
; v0 = vertex position
; v1 = blend weights
; v2 = matrix indices
; v3 = vertex normal
; v4 = uv coord

; temp registers
; r0 = final blended vertex position
; r1 = temp vector
; r3 = temp vector for normal
; r11= final blended normal

;--------------------------------------------------------------------

vs.1.1


;----------------------------
; process first weight
;----------------------------
; get the index
mov a0.x, v2.x

; transform the vertex by this bone
dp4 r1.x, v0, c[a0.x + 0]
dp4 r1.y, v0, c[a0.x + 1]
dp4 r1.z, v0, c[a0.x + 2]

; multiply the result with the weight
mul r0, r1.xyz, v1.x


; transform the normal by this bone, and multiply by weight
dp3 r3.x, v3, c[a0.x + 0]
dp3 r3.y, v3, c[a0.x + 1]
dp3 r3.z, v3, c[a0.x + 2]
mul r11, r3.xyz, v1.x



;----------------------------
; process second weight
;----------------------------
; get the index
mov a0.x, v2.y

; transform the vertex by this bone
dp4 r1.x, v0, c[a0.x + 0]
dp4 r1.y, v0, c[a0.x + 1]
dp4 r1.z, v0, c[a0.x + 2]

; multiply the result with the weight, and add it to final result
mad r0, r1.xyz, v1.y, r0


; transform the normal by this bone, and multiply by weight
dp3 r3.x, v3, c[a0.x + 0]
dp3 r3.y, v3, c[a0.x + 1]
dp3 r3.z, v3, c[a0.x + 2]
mad r11, r3.xyz, v1.y, r11



;----------------------------
; process third weight
;----------------------------
; get the index
mov a0.x, v2.z

; transform the vertex by this bone
dp4 r1.x, v0, c[a0.x + 0]
dp4 r1.y, v0, c[a0.x + 1]
dp4 r1.z, v0, c[a0.x + 2]

; multiply the result with the weight, and add it to final result
mad r0, r1.xyz, v1.z, r0


; transform the normal by this bone, and multiply by weight
dp3 r3.x, v3, c[a0.x + 0]
dp3 r3.y, v3, c[a0.x + 1]
dp3 r3.z, v3, c[a0.x + 2]
mad r11, r3.xyz, v1.z, r11

; R11 CONTAINS THE NORMAL NOW

; normalize the normal
dp3 r11.w, r11, r11
rsq r11.w, r11.w
mul r11, r11, r11.w


;----------------------------
; output the final position
;----------------------------
; set the w value to 1.0
sge r0.w, v0, v0

; transform the final position into clipping space
dp4 oPos.x, r0, c60
dp4 oPos.y, r0, c61
dp4 oPos.z, r0, c62
dp4 oPos.w, r0, c63




;----------------------------
; calculate the vertex color
;----------------------------
; dot normal with light direction in model space
dp3 r0, r11, c65
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
dp3 r0, r11, c75
max r0, r0, c68.x        ; zero out negative values

; multiply diffuse component by material color and store
mul r0, r0.x, c70
mul r0, r0, c76			; multiply by the light color
add r2, r2, r0




; third light
dp3 r0, r11, c77
max r0, r0, c68.x        ; zero out negative values

; multiply diffuse component by material color and store
mul r0, r0.x, c70
mul r0, r0, c78
add r2, r2, r0

mov oD0, r2



;----------------------------
; output texture coord
;----------------------------

mov oT0, v4