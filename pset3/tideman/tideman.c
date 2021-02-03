#include <cs50.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(name, candidates[i]) == 0)
        {
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for (int i = 0; i < candidate_count - 1; i++)
    {
        int winner = ranks[i];

        for (int j = i + 1; j < candidate_count; j++)
        {
            int loser = ranks[j];
            preferences[winner][loser] += 1;
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            if (preferences[i][j] > 0 && preferences[i][j] > preferences[j][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pair_count++;
            }
        }
    }
    return;
}

// Recursively sort the given array of pairs using Merge Sort algorithm
// void merge_sort(pair list[], int size)
// {
//     if (size == 1)
//     {
//         return;
//     }
//     int middle = size / 2;

//     // Sort the left half
//     int left_size = middle;
//     pair left_half[left_size];

//     for (int i = 0; i < left_size; i++)
//     {
//         left_half[i] = list[i];
//     }

//     merge_sort(left_half, left_size);

//     // Sort the right half
//     int right_size = size - middle;
//     pair right_half[right_size];

//     for (int i = 0; i < right_size; i++)
//     {
//         right_half[i] = list[i + middle];
//     }

//     merge_sort(right_half, right_size);

//     // Use a temporary array for sorting and merging both halves
//     pair temp[size];
//     int l = 0;
//     int r = 0;
//     int temps = 0;
//     while (l < left_size || r < right_size)
//     {
//         int left = l < left_size ? left_half[l].winner : -1;
//         int right = r < right_size ? right_half[r].winner : -1;

//         int prefL;
//         int prefR;

//         if (left == -1 || right == -1)
//         {
//             prefL = left == -1 ? -1 : 1;
//             prefR = left == -1 ? 1 : -1;
//         }
//         else
//         {
//             prefL = preferences[left][right];
//             prefR = preferences[right][left];
//         }

//         if (prefL > prefR)
//         {
//             temp[temps] = left_half[l++];
//         }
//         else
//         {
//             temp[temps] = right_half[r++];
//         }
//         temps++;
//     }

//     // Merge sorted array into original array
//     for (int i = 0; i < size; i++)
//     {
//         list[i] = temp[i];
//     }
// }

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    // merge_sort(pairs, pair_count);
    for (int i = 0; i < pair_count - 1; i++)
    {
        int max = i;
        int prefM = preferences[pairs[i].winner][pairs[i].loser];

        for (int j = i + 1; j < pair_count; j++)
        {
            int prefJ = preferences[pairs[j].winner][pairs[j].loser];

            if (prefJ > prefM)
            {
                max = j;
            }
        }

        if (max != i)
        {
            pair temp = pairs[i];
            pairs[i] = pairs[max];
            pairs[max] = temp;
        }
    }
    return;
}

// Check if there's a possible cycle for the next lock
bool check_cycle(int origin, int winner, int find)
{
    for (int i = 0; i < candidate_count; i++)
    {
        // if (i != origin && i != winner)
        if (i != winner)
        {
            if (locked[winner][i] == true)
            {
                return i == find ? true : check_cycle(winner, i, find);
            }
        }
    }
    return false;
}


// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    for (int i = 0; i < pair_count; i++)
    {
        pair current = pairs[i];
        if (!check_cycle(current.winner, current.loser, current.winner))
        {
            locked[current.winner][current.loser] = true;
        }
    }
    return;
}

// Print the winner of the election
void print_winner(void)
{
    bool has_edge[candidate_count];

    for (int i = 0; i < candidate_count; i++)
    {
        has_edge[i] = false;
    }

    for (int i = 0; i < pair_count; i++)
    {
        pair current = pairs[i];
        printf("Locked %s -> %s = %s\n", candidates[current.winner], candidates[current.loser], locked[current.winner][current.loser] ? "true" : "false");
        if (locked[current.winner][current.loser])
        {
            has_edge[current.loser] = true;
        }
    }

    for (int i = 0; i < candidate_count; i++)
    {
        if (!has_edge[i])
        {
            printf("%s\n", candidates[i]);
            break;
        }
    }

    return;
}

