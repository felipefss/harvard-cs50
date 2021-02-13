from cs50 import get_string
import re
from functools import reduce


def main():
    text = get_string("Text: ")

    # Count letters
    letters = reduce(lambda acc, char: acc + 1 if 'a' <= char.lower() <= 'z' else acc, text, 0)

    # Get a list with all words
    words = text.split(" ")

    # Get a list with all sentences. The second step is necessary to remove the empty string at the end
    sentences = re.split(r'[.!?]', text)
    sentences = sentences[:-1]

    L = avg_per_n_words(letters, len(words))
    S = avg_per_n_words(len(sentences), len(words))

    # Calculate Coleman-Liau index with the values above
    index = (0.0588 * L) - (0.296 * S) - 15.8

    if index < 1:
        print("Before Grade 1")
    elif index > 16:
        print("Grade 16+")
    else:
        print(f"Grade {round(index)}")


# Return the average number of letters/sentences per 100 words in the text
def avg_per_n_words(desired, words):
    return (desired * 100) / words


# Run program
main()