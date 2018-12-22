
#include "tgahelpers.h"
#include "../viewer/TextureResource.h"
#include <cstdio>
#include <cstring>



bool WriteTexture (const TextureResource* texture, const char* filename)
{
    FILE* fout = fopen(filename, "wb");
    if (fout == NULL)
        return false;      
	
    const int sizeInBytes = sizeof(TextureResource) + texture->dataSize;
    const int bytesWritten = (int)fwrite (texture, 1, sizeInBytes, fout);
        
    fclose(fout);
    return (bytesWritten == sizeInBytes);
}


void CalculateOutputFilename (char* outFile, const char* inFile)
{
    strcpy (outFile, inFile);
    char* ext = strrchr(outFile, '.');
    if (ext == NULL)
        strcat (outFile, ".texture");
    else
        strcpy (ext, ".texture");

}


int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf ("Usage: converter file.tga\n");
        printf ("       Converts a tga file (without compression) to a binary texture ready to be\n");
        printf ("       used in the runtime. The output file is file.tga.texture.\n");
        return 1;
    }
    
    const char* srcFile = argv[1];
    TextureResource* texture = LoadTGA(srcFile);
    if (texture == NULL)
    {
        printf ("Could not load %s. File could be missing or using TGA comppression.\n", srcFile);
        return 1;
    }
        
    char destFile[256];
    CalculateOutputFilename(destFile, srcFile);
    
    if (!WriteTexture(texture, destFile))
    {
        printf ("Could not write %s texture.\n", destFile);
        delete [] texture;
        return 1;
    }
    
    delete [] texture;
    
    return 0;
}
