#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// File system block size in bytes
const int BLOCK_SIZE = 512;

int main(int argc, char *argv[])
{
    // We can only have 1 argument, which is the input file name
    if (argc != 2)
    {
        printf("Usage: ./recover image\n");
        return 1;
    }

    FILE *input = fopen(argv[1], "r");
    if (input == NULL)
    {
        printf("Can't open file %s\n", argv[1]);
        return 1;
    }

    FILE *output;
    output = NULL;

    // Count the number of files being written
    int file_count = 0;
    char filename[8];

    uint8_t buffer[BLOCK_SIZE];
    while (fread(buffer, sizeof(uint8_t), BLOCK_SIZE, input))
    {
        // Check if first 4 bytes correspond to JPEG signature
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && buffer[3] >> 4 == 0xe)
        {
            // Close output file, if there's one open
            if (output != NULL)
            {
                fclose(output);
                output = NULL;
            }

            // Open a new file, and give it a name with the next file count
            sprintf(filename, "%03d.jpg", file_count++);
            output = fopen(filename, "w");

            if (output == NULL)
            {
                printf("Can't open file to write: %03d.jpg\n", file_count);
                return 1;
            }
        }

        // If there is a JPEG file opened, write whatever is in the buffer in it
        if (output != NULL)
        {
            fwrite(buffer, sizeof(uint8_t), BLOCK_SIZE, output);
        }
    }

    // Closes any opened files
    if (output != NULL)
    {
        fclose(output);
    }
    fclose(input);
}
