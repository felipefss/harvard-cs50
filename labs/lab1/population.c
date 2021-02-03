#include <cs50.h>
#include <stdio.h>

//Say we have a population of n llamas. Each year, n / 3 new llamas are born, and n / 4 llamas pass away.
int calc_end_year(int, int);

int main(void)
{
    // Prompt user for start and end population sizes
    int start;
    do {
        start = get_int("Start size: ");
    } while (start < 9);

    int end;
    do {
        end = get_int("End size: ");
    } while (end < start);

    int years = calc_end_year(start, end);

    printf("\nStart size: %i\nEnd size: %i\nYears: %i\n", start, end, years);
}

int calc_end_year(int start_size, int end_size)
{
    int years = 0;
    int population = start_size;
    do {
        population += (population / 3) - (population / 4);
        years++;
    } while (population < end_size);

    return years;
}