/****************************************************************************
 * recover.c
 * 
 *
 * Recovers JPEGs from a corrupt memory card.
 * 
 * Author: Julie Huang, julie@juliehuang.co.nz
 * 
 * This file is a solution of the Recover problem from
 *   pset4 of CS50x.
 ***************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <cs50.h>

// define BYTE as unsigned 8 bit integer
typedef uint8_t BYTE;

int main (int argc, char* argv[])
{
    // ensure correct usage - only takes 2 command line arguements
    if (argc != 2) 
    {
        fprintf(stderr, "Usage: ./recover file format\n");
        return 1; // error code
    }
    
    char* infile = argv[1]; // remember infile
    char outfile[8]; // array to store name of outfile
    FILE *outptr = NULL; // initialize outfile to NULL
    
    int jpgName = 0; // tracker for naming Jpegs
    
    BYTE *buffer = NULL; // create buffer, initialize to NULL
    buffer = malloc(sizeof(BYTE)*512); // malloc 512 bytes for buffer
    
    // open infile
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL) 
    {
        fprintf(stderr, "Could not open %s.\n", "card.raw");
        return 2; // error code
    }
    
    // keep looping while not end of file and while buffer reads 512 bytes
    while (! feof(inptr) && fread(buffer, sizeof(BYTE), 512, inptr) == 512)
    {
        // if jpeg signature is detected do following:
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] == 0xe0 || buffer[3] == 0xe1))
        {
            // close open outfile, if open
            if (outptr != NULL)
            {
                fclose(outptr);
            }
                
            // create new outfile and write buffer to it
            sprintf(outfile,"%03d.jpg",jpgName); // name jpeg
            outptr = fopen(outfile,"w"); // open outfile
            fwrite(buffer,sizeof(BYTE)*512,1,outptr); // write to outfile
            jpgName++; // track number of jpegs
        }
        else
        {
            // continuously write to outfile, if first jpeg has been detected
            if (jpgName != 0)
            fwrite(buffer,sizeof(BYTE)*512,1,outptr);
        }
    }
    
    // if buffer reads < 512 bytes, write 1 byte at a time until EOF
    while (! feof(inptr))
    {
        fread(buffer, sizeof(BYTE), 1, inptr); // read
        fwrite(buffer,sizeof(BYTE), 1, outptr); // write
    }

    free(buffer); // free malloc
}