#include <cs50.h>
#include <stdio.h>
#include <math.h>

bool isValidCardLength(int);
string getCardType(long);
int getNumLength(long);
bool checksum(long);

int main(void)
{
    long cardNumber = get_long("Number: ");
    int length = getNumLength(cardNumber);

    // Check card number length and run checksum algorithm
    if (!isValidCardLength(length) || !checksum(cardNumber))
    {
        printf("INVALID\n");
        return 0;
    }

    printf("%s\n", getCardType(cardNumber));

}

int getNumLength(long num)
{
    return floor(log10(num)) + 1;
}

// American Express uses 15-digit numbers, MasterCard uses 16-digit numbers, and Visa uses 13- and 16-digit numbers
bool isValidCardLength(int length)
{
    return length == 13 || length == 15 || length == 16;
}

// All American Express numbers start with 34 or 37; most MasterCard numbers start with 51, 52, 53, 54, or 55 and all Visa numbers start with 4.
string getCardType(long number)
{
    long num = number;
    int len = getNumLength(number);
    int firstTwo = 0;
    while (--len)
    {
        num /= 10;
        if (len == 2)
        {
            firstTwo = num;
            break;
        }
    }

    if (firstTwo == 34 || firstTwo == 37)
    {
        return "AMEX";
    }
    if (firstTwo >= 51 && firstTwo <= 55)
    {
        return "MASTERCARD";
    }
    if (firstTwo >= 40 && firstTwo <= 49)
    {
        return "VISA";
    }

    return "INVALID";
}


// Luhn's algorithm
// 1. Multiply every other digit by 2, starting with the number’s second-to-last digit, and then add those products’ digits together.
// 2. Add the sum to the sum of the digits that weren’t multiplied by 2.
// 3. If the total’s last digit is 0 (or, put more formally, if the total modulo 10 is congruent to 0), the number is valid!
bool checksum(long number)
{
    long num = number;
    int len = getNumLength(number);
    int i = 0;
    int sum = 0;

    while (len--)
    {
        int digit = num % 10;

        // For every other digit, multiply it by 2
        if (i % 2 != 0)
        {
            int prod = digit * 2;
            if (getNumLength(prod) > 1)
            {
                // Sum both digits of the product, in case it's more than 1
                sum += prod % 10;
                prod /= 10;
                sum += prod % 10;
            }
            else
            {
                sum += prod;
            }
        }
        else
        {
            sum += digit;
        }

        num /= 10;
        i++;
    }

    return sum % 10 == 0;
}