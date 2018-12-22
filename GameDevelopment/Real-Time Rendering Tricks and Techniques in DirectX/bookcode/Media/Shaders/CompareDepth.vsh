vs.1.1

;Output the position
dp4 oPos.x, v0, c0    
dp4 oPos.y, v0, c1    
dp4 oPos.z, v0, c2    
dp4 oPos.w, v0, c3

;Compute the distance from the vertex to the light   
dp4 r1.z, v0, c12     
 
;subtract the near plane distance from r1.z (the distance)
sub r1.z, r1.z, c4.x

;multiply the resulting distance by the scaling factor
mul r1.z, r1.z, c4.y

;Output the 8bit distance into the specular color (v1 in the pixel shader)
mov oD1,  r1.zzzz

;Output the diffuse color based on simple diffuse lighting
dp3 r0, -c5, v3
mad oD0, v5, r0, c6
 
;Compute the texture coordinates         
dp4 r2.x, v0, c20      
dp4 r2.y, v0, c21     
dp4 r2.z, v0, c22    
dp4 r2.w, v0, c23     
mov oT0,  r2
