/*******************************************************************
 * resize.c
 * 
 * 
 * Resizes a bitmap (bmp) image by a factor of "n"
 * 
 * Author: Julie Huang, julie@juliehuang.co.nz
 * 
 * This file is a solution of the Resize problem from
 *   pset4 of CS50x.
 ********************************************************************/
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: ./resize n infile outfile\n");
        return 1;
    }

    // cast argv[1] to int
    int n = atoi(argv[1]);
    
    // check n is a positive int <= 100
    if (n < 1 || n > 100)
    {
        fprintf(stderr, "n must be a positive integer between 1 and 100 (inclusive).\n");
        return 2;
    }
    
    // remember file names
    char *infile = argv[2]; 
    char *outfile = argv[3];

    // open input file 
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 3; // error code
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 4;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 5;
    }

    // create outfile's BITMAPFILEHEADER
    BITMAPFILEHEADER bfNew = bf;
    
    // create outfile's BITMAPINFOHEADER
    BITMAPINFOHEADER biNew = bi;
    biNew.biWidth = bi.biWidth * n; 
    biNew.biHeight = bi.biHeight * n;
    
    // padding for input scanlines
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    // padding for output scanlines
    int paddingNew = (4 - (biNew.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    // calculate biNew's image size
    biNew.biSizeImage = (biNew.biWidth * sizeof(RGBTRIPLE) + paddingNew) * abs(biNew.biHeight);
    // calculate bfNew's file size
    bfNew.bfSize = biNew.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // write output's BITMAPFILEHEADER
    fwrite(&bfNew, sizeof(BITMAPFILEHEADER), 1, outptr);
    
    // write output's BITMAPINFOHEADER
    fwrite(&biNew, sizeof(BITMAPINFOHEADER), 1, outptr);

    // Allocate buffer memory for new scanline, set pointer to NULL
    RGBTRIPLE* buffer = NULL;
    buffer = malloc(sizeof(RGBTRIPLE) * (biNew.biWidth));
    
    // iterate over infile's scanlines
    for(int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        int tracker = 0;
        // iterate over pixels in infile scanline
        for(int j = 0; j < bi.biWidth; j++)
        {
            // temporary storage
            RGBTRIPLE triple;
            
            // read RGBTRIPLE triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
            
            for(int k = 0; k < n; k++)
            {
                // Horizontally write triple to buffer n times
                *(buffer + (tracker)) = triple;
                tracker++;
            }
        }
        
        // skip over original padding (if any)
        fseek(inptr, padding, SEEK_CUR);
            
        // Vertically write buffer n times
        for(int x = 0; x < n; x++)
        {
            fwrite((buffer), sizeof(RGBTRIPLE), biNew.biWidth, outptr);
                
            // add padding to end of new scanline (if any)
            for(int y = 0; y < paddingNew; y++)
                fputc(0x00, outptr);
        }
    }
    
    // free allocated memory at buffer
    free(buffer);

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
