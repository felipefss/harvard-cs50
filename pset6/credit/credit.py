from cs50 import get_int


def main():
    # Get user's input and convert it to string, for better handling
    cardNumber = str(get_int("Number: "))

    # Check card number length and run checksum algorithm
    if (not isValidCardLength(cardNumber)) or (not checksum(cardNumber)):
        print("INVALID")
    else:
        print(getCardType(cardNumber))


# American Express uses 15-digit numbers, MasterCard uses 16-digit numbers, and Visa uses 13- and 16-digit numbers
def isValidCardLength(num):
    length = len(num)
    return length == 13 or length == 15 or length == 16


# All American Express numbers start with 34 or 37;
# most MasterCard numbers start with 51, 52, 53, 54, or 55 and
# all Visa numbers start with 4.
def getCardType(num):
    firstTwo = int(num[:2])

    if firstTwo == 34 or firstTwo == 37:
        return "AMEX"
    if 51 <= firstTwo <= 55:
        return "MASTERCARD"
    if 40 <= firstTwo <= 49:
        return "VISA"

    return "INVALID"


# Luhn's algorithm
# 1. Multiply every other digit by 2, starting with the number’s second-to-last digit, and then add those products’ digits together.
# 2. Add the sum to the sum of the digits that weren’t multiplied by 2.
# 3. If the total’s last digit is 0 (or, put more formally, if the total modulo 10 is congruent to 0), the number is valid!
def checksum(num):
    reversedNum = num[::-1]
    total = 0

    for i in range(len(reversedNum)):
        # Check if current digit is on an odd index
        if i % 2 != 0:
            prod = int(reversedNum[i]) * 2

            # add product's digits together, if more than one digit
            # otherwise, only add the product to the total
            digits = str(prod)
            total += sum([int(d) for d in digits])

        else:
            # if digit is on even index, just add it to the total
            total += int(reversedNum[i])

    return total % 10 == 0


# Run main function
main()