//
// Perspective Shadow Maps
// accompanying example code for Game Programming Gems 4
// (w)(c) 2003, Carsten Dachsbacher (dachsbacher@cs.fau.de), Marc Stamminger (stamminger@cs.fau.de)
//


#ifndef USE_P_BUFFER
void    createShadowTextureMap()
{
    // create shadow map
    glGenTextures( 1, &shadowDepthMap );
    glBindTexture( GL_TEXTURE_2D, shadowDepthMap );

    // define texture format
    glCopyTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0, SHADOWSIZE, SHADOWSIZE, 0 );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    
    // and setup depth comparison
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glTexParameteri( GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_LUMINANCE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL );
}
#endif

void    renderShadowMap()
{
    // the viewport setting is actually not required, when using a p-buffer
#ifndef USE_P_BUFFER
    int viewPort[ 4 ];

    glGetIntegerv( GL_VIEWPORT, viewPort );
    glViewport( 0, 0, SHADOWSIZE, SHADOWSIZE );
#endif

    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();

    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadMatrixf( (float*)&matrixS );
    
    glDisable( GL_LIGHTING );
    glDisable( GL_TEXTURE_2D );

#ifndef USE_P_BUFFER
    // for display of the p-buffer content
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
#endif

    glEnable( GL_POLYGON_OFFSET_FILL );
    
    renderScene();

#ifndef USE_P_BUFFER
    glBindTexture( GL_TEXTURE_2D, shadowDepthMap );
    glCopyTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, 0, 0, SHADOWSIZE, SHADOWSIZE );
#endif

    glEnable( GL_LIGHTING );
    glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

    glDisable( GL_POLYGON_OFFSET_FILL );
    
#ifndef USE_P_BUFFER
    glViewport( viewPort[0], viewPort[1], viewPort[2], viewPort[3] );
#endif

    glMatrixMode( GL_PROJECTION );
    glPopMatrix();

    glMatrixMode( GL_MODELVIEW );
    glPopMatrix();
}



void    setupShadowTexCoordGeneration()
{
    // for the sake of simplicity, we use the same setup code
    // for vertex program and fixed pipeline rendering
    static float genS[] = { 1.0f, 0.0f, 0.0f, 0.0f };
    static float genT[] = { 0.0f, 1.0f, 0.0f, 0.0f };
    static float genR[] = { 0.0f, 0.0f, 1.0f, 0.0f };
    static float genQ[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    
    glEnable( GL_TEXTURE_2D );                              

    glEnable( GL_TEXTURE_GEN_S );                           
    glEnable( GL_TEXTURE_GEN_T );                           
    glEnable( GL_TEXTURE_GEN_R );                           
    glEnable( GL_TEXTURE_GEN_Q );                           
    glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR );  
    glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR );  
    glTexGeni( GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR );  
    glTexGeni( GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR );  
    glTexGenfv( GL_S, GL_EYE_PLANE, genS );                 
    glTexGenfv( GL_T, GL_EYE_PLANE, genT );                 
    glTexGenfv( GL_R, GL_EYE_PLANE, genR );                 
    glTexGenfv( GL_Q, GL_EYE_PLANE, genQ );                 

    glMatrixMode( GL_TEXTURE );                             
    glLoadIdentity();           

    glTranslatef ( 0.5f, 0.5f, 0.5f );
    glScalef     ( 0.5f, 0.5f, 0.5f );                          

    glMultMatrixf( (float*)&matrixS );

#ifndef USE_P_BUFFER
    glBindTexture( GL_TEXTURE_2D, shadowDepthMap );
#endif

    glMatrixMode( GL_MODELVIEW );
}


void    clearShadowTexCoordGeneration()
{
    glMatrixMode( GL_TEXTURE );             
    glLoadIdentity();                       
                                            
    glDisable( GL_TEXTURE_GEN_S );          
    glDisable( GL_TEXTURE_GEN_T );          
    glDisable( GL_TEXTURE_GEN_R );          
    glDisable( GL_TEXTURE_GEN_Q );          
                                        
    glMatrixMode( GL_MODELVIEW );
}
