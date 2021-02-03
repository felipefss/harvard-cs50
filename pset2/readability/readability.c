#include <cs50.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

float get_average_base_words(int, int);

int main(void)
{
    // Retrieve user's input
    string text = get_string("Text: ");

    int lettersCount = 0;
    int wordsCount = 0;
    int sentencesCount = 0;

    for (int i = 0; text[i] != '\0'; i++)
    {
        char c = toupper(text[i]);

        if (c >= 'A' && c <= 'Z')
        {
            // Count how many letters are in the text
            lettersCount++;
        }
        else if (c == ' ' || (c == '\'' && text[i + 1] != 's'))
        {
            // Count how many words there are
            wordsCount++;
        }

        if (c == '.' || c == '!' || c == '?')
        {
            // Count how many sentences there are
            sentencesCount++;
        }
    }
    // Count last word
    wordsCount++;
    printf("%i letter(s)\n", lettersCount);
    printf("%i word(s)\n", wordsCount);
    printf("%i sentence(s)\n", sentencesCount);

    float L = get_average_base_words(lettersCount, wordsCount);
    float S = get_average_base_words(sentencesCount, wordsCount);

    float index = (0.0588 * L) - (0.296 * S) - 15.8;

    if (index < 1)
    {
        printf("Before Grade 1\n");
    }
    else if (index > 16)
    {
        printf("Grade 16+\n");
    }
    else
    {
        printf("Grade %.0f\n", index);
    }
}

// Return the average number of letters per 100 words in the text
float get_average_base_words(int val, int words)
{
    return (val * 100) / words;
}