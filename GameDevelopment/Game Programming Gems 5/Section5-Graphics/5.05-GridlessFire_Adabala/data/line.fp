!!ARBfp1.0

################################################
                                                                                
# Demo for chapter "Gridless Controllable Fire"
# in Games Programming Gems 5.
                                                                                
# Author: Neeharika Adabala
                                                                                
# Date: August 2004
                                                                                
################################################


PARAM param = { 0.001, -0.007,  0.001, 0.0 };

TEMP src, coord0, coord1, mask, tcoord, new, one, two, temp;

# source texture
TEX src, fragment.texcoord[0], texture[1], 2D;

# displace in +ve direction
TEX coord0, tcoord, texture[2], 2D;
MAD coord0, coord0, 0.01, fragment.texcoord[0];

# displace in -ve direction
TEX coord1, tcoord, texture[3], 2D;
#ADD coord1.y, coord1.y, src.x;
MAD coord1, coord1, param, fragment.texcoord[0];

# Blur off st values
TEX new, fragment.texcoord[0], texture[4], 2D;

TEX one, coord0, texture[1], 2D;

TEX two, coord1, texture[4], 2D;

MAD new, one, 0.9, new;
MAD new, two, 0.9, new;
MUL new, new, 0.5;

MOV result.color, new;

END
