//
// Perspective Shadow Maps
// accompanying example code for Game Programming Gems 4
// (w)(c) 2003, Carsten Dachsbacher (dachsbacher@cs.fau.de), Marc Stamminger (stamminger@cs.fau.de)
//

// read shader in a ascii file and return a pointer to the buffer
unsigned char *readShaderFile( char *name )
{
    FILE *f = fopen( name, "rt" );

    if( f == NULL ) return NULL;

    fseek( f, 0, SEEK_END );
    int fsize = ftell( f );
    fseek( f, 0, SEEK_SET );

    unsigned char *shader = new unsigned char[ fsize ];

    int s = fread( shader, 1, fsize, f );

    shader[ s ] = 0;

    fclose( f );

    return shader;
}

// generate program ID and pass the program string
void setupProgram( char *program, GLenum target, GLuint *programID )
{
    glGenProgramsARB( 1, programID );
    glBindProgramARB( target, *programID );

    unsigned char *shaderASM = readShaderFile( program );

    glProgramStringARB( target, GL_PROGRAM_FORMAT_ASCII_ARB,
                        strlen( (char*)shaderASM ), shaderASM );

    int ep;
    glGetIntegerv( GL_PROGRAM_ERROR_POSITION_ARB, (int*)&ep );

    if ( ep != -1 )
    {
        char buf[ 512 ];
        sprintf( buf, "error in program at offset %d.\nprogram will exit...", ep );
        MessageBox( NULL, buf, "[error]", MB_OK );
        exit( 1 );
    }

    delete shaderASM;
}
