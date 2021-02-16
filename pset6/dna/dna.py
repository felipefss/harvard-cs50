from sys import argv
from sys import exit
import csv

people = {}


def main():
    if len(argv) != 3:
        print("Usage: python dna.py <CSV_DATABASE> <TXT_SEQUENCES>")
        exit(1)

    with open(argv[1]) as csv_file:
        reader = csv.reader(csv_file)

        # # Get STRs information
        str_list = next(reader)[1:]
        str_count = {s: 0 for s in str_list}

        # Get individuals info
        for row in reader:
            people[row[0]] = {str_list[i]: int(row[i+1]) for i in range(len(str_list))}

    with open(argv[2]) as dna_file:
        # Save dna sequences in memory
        dna = dna_file.read().rstrip()

    # For each STR, find the max repeat in a row
    for s in str_list:
        str_size = len(s)
        index = dna.find(s)
        str_count[s] = find_str_in_seq(dna, s)

    # Print individual who matches the STR count
    print(check_str_match(str_count))


# Given a DNA sequence and an STR, count the maximum times the STR is repeated in back to back
def find_str_in_seq(seq, st):
    count = 0
    max_count = 0
    size = len(st)

    # Find the index, in dna string, where the STR starts
    index = seq.find(st)

    while index != -1:
        # If the searched STR is found, add 1 to the count
        if seq[index:index + size] == st:
            count += 1

        # Update max_count, only if current count is greater than it
        max_count = count if count > max_count else max_count

        # If the next STR is not the one searched for, reset the count
        _next = index + size
        if seq[_next:_next + size] != st:
            count = 0

        # Find the next index where the STR starts
        index = seq.find(st, index + 1)

    return max_count


# Check if the STR count matches exactly with one of the individuals and return their name;
# Otherwise, return "No match"
def check_str_match(str_count):
    # Search if there is any individual with the exact STR count found
    for p in people:
        person = people[p]
        if all([person[key] == str_count[key] for key in str_count]):
            return p
    return "No match"


# Run program
main()