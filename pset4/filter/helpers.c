#include "helpers.h"
#include <math.h>

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            RGBTRIPLE temp = image[i][j];
            int avg = round((temp.rgbtBlue + temp.rgbtGreen + temp.rgbtRed) / 3.0);
            image[i][j].rgbtBlue = avg;
            image[i][j].rgbtGreen = avg;
            image[i][j].rgbtRed = avg;
        }
    }
    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width/2; j++)
        {
            int opposite = width - 1 - j;

            RGBTRIPLE temp = image[i][j];
            image[i][j] = image[i][opposite];
            image[i][opposite] = temp;
        }
    }
    return;
}

RGBTRIPLE get_average_adjacent(int x, int y, int h, int w, RGBTRIPLE image[h][w])
{
    int count = 0;
    int sumR = 0;
    int sumG = 0;
    int sumB = 0;

    for (int i = x - 1; i <= x + 1; i++)
    {
        for (int j = y - 1; j <= y + 1; j++)
        {
            // Make sure we don't access an out of bound pixel
            if (i >= 0 && i < h && j >= 0 && j < w)
            {
                // Sum each color from each valid pixel
                sumR += image[i][j].rgbtRed;
                sumG += image[i][j].rgbtGreen;
                sumB += image[i][j].rgbtBlue;
                count++;
            }
        }
    }

    // Store and return the averages
    RGBTRIPLE output;
    output.rgbtBlue = round(sumB / (float)count);
    output.rgbtGreen = round(sumG / (float)count);
    output.rgbtRed = round(sumR / (float)count);

    return output;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE output_image[height][width];

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            output_image[i][j] = get_average_adjacent(i, j, height, width, image);
        }
    }

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image[i][j] = output_image[i][j];
        }
    }
    return;
}

RGBTRIPLE get_sobel_operator(int x, int y, int h, int w, RGBTRIPLE image[h][w])
{
    // int count = 0;
    int sumRx = 0;
    int sumRy = 0;
    int sumGrx = 0;
    int sumGry = 0;
    int sumBx = 0;
    int sumBy = 0;


    // Sobel kernels
    int Gx[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };

    int Gy[3][3] = {
        {-1, -2, -1},
        {0, 0, 0},
        {1, 2, 1}
    };

    for (int i = x - 1; i <= x + 1; i++)
    {
        // Compute x starting at 0, for kernel array
        int gi = i - (x - 1);

        for (int j = y - 1; j <= y + 1; j++)
        {
            // Compute y starting at 0, for kernel array
            int gj = j - (y - 1);

            // Make sure we don't access an out of bound pixel
            if (i >= 0 && i < h && j >= 0 && j < w)
            {
                sumRx += image[i][j].rgbtRed * Gx[gi][gj];
                sumRy += image[i][j].rgbtRed * Gy[gi][gj];

                sumGrx += image[i][j].rgbtGreen * Gx[gi][gj];
                sumGry += image[i][j].rgbtGreen * Gy[gi][gj];

                sumBx += image[i][j].rgbtBlue * Gx[gi][gj];
                sumBy += image[i][j].rgbtBlue * Gy[gi][gj];
            }
        }
    }

    // Store and return the resulting calculations
    RGBTRIPLE output;
    int blue = round(sqrt(pow(sumBx, 2) + pow(sumBy, 2)));
    int green = round(sqrt(pow(sumGrx, 2) + pow(sumGry, 2)));
    int red = round(sqrt(pow(sumRx, 2) + pow(sumRy, 2)));

    output.rgbtBlue = blue > 255 ? 255 : blue;
    output.rgbtGreen = green > 255 ? 255 : green;
    output.rgbtRed = red > 255 ? 255 : red;

    return output;
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE output_image[height][width];

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            output_image[i][j] = get_sobel_operator(i, j, height, width, image);
        }
    }

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image[i][j] = output_image[i][j];
        }
    }
    return;
}
