#include <cs50.h>
#include <stdio.h>

void print_char(int, char);

int main(void)
{
    // Ask user for pyramid's height, which should be between 1 and 8, inclusive
    int height;
    do
    {
        height = get_int("Height: ");
    }
    while (height < 1 || height > 8);

    for (int i = 1; i <= height; i++)
    {
        int spaces = height - i;

        // Print left side
        print_char(spaces, ' ');
        print_char(i, '#');

        // Space in the middle
        printf("  ");

        // Print right side
        print_char(i, '#');

        printf("\n");
    }
}

void print_char(int n, char c)
{
    for (int i = 0; i < n; i++)
    {
        printf("%c", c);
    }
}