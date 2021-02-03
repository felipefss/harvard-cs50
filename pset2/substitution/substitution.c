#include <cs50.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

bool isInputValid(int, string);
bool isDuplicate(char[], char, int);

int main(int argc, string argv[])
{
    if (!isInputValid(argc, argv[1]))
    {
        return 1;
    }

    string plainText = get_string("plaintext: ");
    // int textSize = sizeof(&plainText) / sizeof(plainText[0]);
    int textSize = strlen(plainText);
    char cipherText[textSize];
    string input = argv[1];
    int i;


    // begin encryption
    for (i = 0; i < textSize; i++)
    {
        char pt = plainText[i];
        // char c = toupper(input[i]);

        int index = toupper(pt) - 'A';
        char output = pt;

        // If current char is a letter, encrypt it
        if ((pt >= 'A' && pt <= 'Z') || (pt >= 'a' && pt <= 'z'))
        {
            output = pt >= 'A' && pt <= 'Z' ? toupper(input[index]) : tolower(input[index]);
        }

        cipherText[i] = output;
    }
    // Finalize output string
    cipherText[i] = '\0';

    printf("ciphertext: %s\n", cipherText);
    return 0;
}

// Check program initialization is valid
bool isInputValid(int argc, string input)
{
    // Exit program if the number of arguments is different than 1
    if (argc != 2)
    {
        printf("Usage: ./substitution key\n");
        return false;
    }

    int length = 0;
    char usedLetters[26] = {};
    bool isValid = true;

    for (int i = 0; input[i] != '\0'; i++)
    {
        char c = toupper(input[i]);

        // If there is something other than letters in input, exit program
        if (c < 'A' || c > 'Z')
        {
            isValid = false;
            break;
        }

        // If there are duplicate letters, exit program
        if (isDuplicate(usedLetters, c, length))
        {
            isValid = false;
            break;
        }
        usedLetters[i] = c;
        length++;
    }

    if (!isValid || length != 26)
    {
        printf("Key should have 26 unique characters\n");
        return false;
    }

    return true;
}

// Check if given letter l has already been added to used array
bool isDuplicate(char used[], char l, int len)
{
    for (int i = 0; i < len; i++)
    {
        if (used[i] == l)
        {
            return true;
        }
    }

    return false;
}