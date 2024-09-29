# Steganography in BMP Files

This project implements a simple steganography technique to embed a secret message into a BMP image file by manipulating the least significant bits (LSBs) of the pixel data.

## Features

- Reads a BMP file and embeds a secret message.
- Outputs a new BMP file with the encoded message.
- Prints the starting position of the pixel array and the dimensions of the image.

## Usage

Compile the code using a C compiler (e.g., `gcc`):

```bash
gcc -o encoder encoder.c
./encoder sheldon.bmp output.bmp "top secret message"
