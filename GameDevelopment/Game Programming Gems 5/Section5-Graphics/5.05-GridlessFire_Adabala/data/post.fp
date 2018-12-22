!!ARBfp1.0

################################################
                                                                                
# Demo for chapter "Gridless Controllable Fire"
# in Games Programming Gems 5.
                                                                                
# Author: Neeharika Adabala
                                                                                
# Date: August 2004
                                                                                
################################################

TEMP value, one, two;

TEX value, fragment.texcoord[0], texture[0], 2D;
ADD value, value, 0.1;
TEX one, value.x, texture[1], 1D;

ADD result.color, one, 0.0;

END
