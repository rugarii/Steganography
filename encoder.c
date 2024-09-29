#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(push, 1)
typedef struct {
  unsigned short bfType;
  unsigned int bfSize;
  unsigned short bfReserved1;
  unsigned short bfReserved2;
  unsigned int bfOffBits;
} BITMAPFILEHEADER;

typedef struct {
  unsigned int biSize;
  int biWidth;
  int biHeight;
  unsigned short biPlanes;
  unsigned short biBitCount;
  unsigned int biCompression;
  unsigned int biSizeImage;
  int biXPelsPerMeter;
  int biYPelsPerMeter;
  unsigned int biClrUsed;
  unsigned int biClrImportant;
} BITMAPINFOHEADER;
#pragma pack(pop)

int main (int argc, char *argv[]) {

  //check if the number  of arguments is correct
  if (argc != 4) {
    printf("Not enough arguments. Usage: %s <input BMP file> <output BMP file> <secret message>\n", argv[0]);
    return 1;
  }

  char *inputFilePath = argv[1];
  char *outputFilePath = argv[2];

  //secret message stored in a variable
  char *secretMessage = argv[3];

  //open input BMP file in binary read mode
  FILE *inputFile = fopen(inputFilePath, "rb");
  if(!inputFile) {
    perror("Error opening input file.");
    return 1;
  }

  //open output BMP file in binary write mode
  FILE *outputFile = fopen(outputFilePath, "wb");
  if(!outputFile) {
    perror("Error opening output file.");
    fclose(inputFile);
    return 1;
  }

  BITMAPFILEHEADER fileHeader;
  fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, inputFile);

  if (fileHeader.bfType != 0x4D42) {
    fprintf(stderr, "Error: Not a valid BMP file.\n");
    fclose(inputFile);
    fclose(outputFile);
    return 1;
  }

  BITMAPINFOHEADER infoHeader;
  fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, inputFile);

  //print starting position of pixel array
  printf("Pixel data starts at: %u\n", fileHeader.bfOffBits);

  //print width and height of image
  printf("Width of image: %d\n", infoHeader.biWidth);
  printf("Height of image: %d\n", infoHeader.biHeight < 0 ? -infoHeader.biHeight : infoHeader.biHeight);

  //calculate image size if not provided and store in variable
  int imageSize = infoHeader.biSizeImage ? infoHeader.biSizeImage : infoHeader.biWidth * abs(infoHeader.biHeight) * 4;
  infoHeader.biSizeImage = imageSize;

  unsigned char *pixelArray = (unsigned char *)malloc(imageSize); //allocate memory to store pixel data.
  if (!pixelArray) {
    perror("Memory allocation failed");
    fclose(inputFile);
    fclose(outputFile);
    return 1;
  }

  fread(pixelArray, imageSize, 1, inputFile); //read pixel data from input file into the allocated memory(pixelArray) after the headers

  int bitIndex = 0; //track position in pixel array

  //loop iterates over all pixels in the image including null terminator
  for (int i = 0; i < strlen(secretMessage) + 1; i++) {
    char currentChar = secretMessage[i];

    //loop iterates over all the bytes in the secret message
    for(int j = 0; j < 8; j++) {
      unsigned char currentPixel = pixelArray[bitIndex / 8];

      if (bitIndex % 32 < 24) {
        currentPixel &= 0xFE; //clears least significant bit (LSB) of pixel
        currentPixel |= (currentChar >> j) & 1; //sets LSB of pixel to corresponding bit from secret message
        pixelArray[bitIndex / 8] =currentPixel; //stores modified pixel back into pixel array
      }
      bitIndex ++;
    }
  }

  //write fileHeader, infoHeader, and modified pixel array to output file
  fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, outputFile);
  fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, outputFile);
  fwrite(pixelArray, imageSize, 1, outputFile);

  free(pixelArray); //frees the memory allocated for the pixel array

  fclose(inputFile);
  fclose(outputFile);

  printf("Secret message embedded successfully into %s.\n", outputFilePath);

  return 0;
}