#include "VM.H"


int main( int argc, char **argv )
{
  // The usage to this program is very simple:  it requires a single file name
  // which is the bytecode stream to execute.
  if ( argc != 2 ) {
    printf( "usage: %s file", argv[0] );
    return 1;
  }

  // Jump through hoops opening the file.  We need to know how big to allocate
  // the buffer, so make sure to seek to the end of the file to get its file
  // size.
  FILE *f = fopen( argv[1], "rb" );
  if ( f == NULL ) {
    perror( argv[1] );
    return 1;
  }

  fseek( f, 0, SEEK_END );
  size_t len = ftell( f );
  rewind( f );

  char *stream = new char [len];

  fread( stream, len, 1, f );

  // Now that we finially have the bytecode stream from the file, create an
  // instance of the VM and execute it.
  VM vm( stream, len );
  vm.Exec();

  delete [] stream;
  fclose( f );

  return 0;
}
