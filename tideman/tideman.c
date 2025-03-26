#include <cs50.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9
const int START = 0;
// preferences[i][col] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
} pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
bool alreadyUsed(int arr[], int num, int len);
void record_preferences(int ranks[]);
int findHighestVote(int array[][MAX], int candidateCount);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);
void printTwoDimBoolArray(bool arr[][MAX], int candidateCount);
void printTwoDimensionalArray(int arr[][MAX], int candidateCount);
bool cycleCheck(int n, pair pair[], int currentLoser, int CurrentWinner, int Counter);
int winnerCheck(int col, bool arr[][MAX]);

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

    printf("\n\npreferences table\n\n");
    printTwoDimensionalArray(preferences, candidate_count);

    add_pairs();
    sort_pairs();
    lock_pairs();

    printf("\n\nlocked in results\n\n");
    printTwoDimBoolArray(locked, candidate_count);

    printf("\n\nnow print whoever has no arrows pointing at them\n\n");
    print_winner();
    return 0;
}

// print two dimensional array
void printTwoDimensionalArray(int arr[][MAX], int candidateCount)
{

    int len = candidateCount - 1;

    for (int i = 0; i <= len; i++)
    {
        for (int j = 0; j <= len; j++)
        {
            printf("[%i][%i] = %i\n", i, j, arr[i][j]);
        }
    }
    printf("\n\n");
    return;
}

// print two dimensional bool array
void printTwoDimBoolArray(bool arr[][MAX], int candidateCount)
{

    int len = candidateCount - 1;

    for (int i = 0; i <= len; i++)
    {
        for (int j = 0; j <= len; j++)
        {
            printf("[%i][%i] = %i\n", i, j, arr[i][j]);
        }
    }
    printf("\n\n");
    return;
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

// Update preferences given ONE voter's ranks
void record_preferences(int ranks[])
{
    printf("recording preferences...\n");

    // local var stores who THIS ONE voter has already voted for
    int prevVotes[candidate_count];
    // int to increment through ranks array
    int index = 0;

    for (int i = 0; i < candidate_count && index < candidate_count - 1; i++)
    {
        if (i == ranks[index])
        {
            prevVotes[index] = i;

            for (int j = 0; j < candidate_count; j++)
            {
                if (i != j && !alreadyUsed(prevVotes, j, index))
                {
                    preferences[i][j]++;
                    printf("preferences[%i][%i] = %i\n", i, j, preferences[i][j]);
                }
            }
            index++;
            i = -1;
        }
    }
    return;
}

// FUNCTION find integer for highest number of votes
int findHighestVote(int array[][MAX], int candidateCount)
{
    int len = candidateCount - 1;
    int highestVote = 0;

    for (int i = 0; i <= len; i++)
    {
        for (int j = 0; j <= len; j++)
        {
            if (array[i][j] > highestVote)
            {
                highestVote = array[i][j];
            }
        }
    }
    return highestVote;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    printf("adding pairs...\n\n");

    printf("DRAWS ARE NOT RECORDED!!!\n\n");
    int index = 0;

    for (int row = 0; row < candidate_count; row++)
    {
        for (int col = 0; col < candidate_count; col++)
        {
            if (row != col && preferences[row][col] > preferences[col][row])
            {
                pairs[index].winner = row;
                pairs[index].loser = col;
                printf("pairs[%i].winner = %i\n", index, row);
                printf("pairs[%i].loser = %i\n", index, col);
                index++;
            }
        }
    }
    pair_count = index;
    return;
}

// FUNCTION returns true; if integer found in array
bool alreadyUsed(int arr[], int num, int len)
{
    for (int i = 0; i <= len; i++)
    {
        if (num == arr[i])
        {
            return true;
        }
    }
    return false;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    printf("sorting pairs...\n\n");

    int highestVote = findHighestVote(preferences, candidate_count);
    printf("highestvote = %i\n\n", highestVote);

    int indexCounter = 0;
    int sortedWinners[pair_count];
    int sortedLosers[pair_count];

    do
    {
        for (int i = 0; i < pair_count; i++)
        {
            if (preferences[pairs[i].winner][pairs[i].loser] == highestVote)
            {
                sortedWinners[indexCounter] = pairs[i].winner;
                sortedLosers[indexCounter] = pairs[i].loser;
                indexCounter++;
            }
        }
        highestVote -= 1;
    }
    while (highestVote > 0);

    printf("sorted pairs:\n");

    // overwrite pairs with sorted winners/losers
    for (int i = 0; i < pair_count; i++)
    {
        pairs[i].winner = sortedWinners[i];
        pairs[i].loser = sortedLosers[i];
        printf("pairs[%i].winner = %i\n", i, pairs[i].winner);
        printf("pairs[%i].loser = %i\n", i, pairs[i].loser);
    }
    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    printf("locked pairs...\n\n");

    // lock first one in for sure (no cycles possible yet)
    locked[pairs[START].winner][pairs[START].loser] = true;
    printf("1st locked[%i][%i] = %d\n\n", pairs[START].winner, pairs[START].loser,
           locked[pairs[START].winner][pairs[START].loser]);

    int counter = 0;

    // iterating through the pairs starting at 2nd pair passing them through cycle check
    for (int pairNumber = 1; pairNumber < pair_count; pairNumber++)
    {
        int winner = pairs[pairNumber].winner;
        int loser = pairs[pairNumber].loser;
        printf("checking next winner = %i\n", winner);
        printf("checking next loser = %i\n", loser);

        // if cycle check is false then lock in the edge
        if (!cycleCheck(pairNumber, pairs, loser, winner, counter))
        {
            printf("locking in [%i][%i]\n", winner, loser);
            locked[winner][loser] = true; // true = 1
            printf("locked[%i][%i] = %d\n\n", winner, loser, locked[winner][loser]);
        }
    }
    return;
}
// if we can trace back a path from current loser being a winner there is a cycle
// can we trace CURRENT loser back to itself
bool cycleCheck(int pairNumber, pair pair[], int currentLoser, int currentWinner, int Counter)
{
    // base case
    if (Counter > pair_count || (Counter >= 2 && currentWinner == currentLoser))
    {
        printf("cycle found\n\n");

        return true;
    }

    for (int prevPairNumb = pairNumber - 1; prevPairNumb >= 0; prevPairNumb--)
    {
        int previousLoser = pair[prevPairNumb].loser;
        int previousWinner = pair[prevPairNumb].winner;

        // recursive case
        if (currentLoser == previousWinner)
        {
            printf("yes found Loser (%i) as the %ith Winner (%i)\n\n", currentLoser, prevPairNumb,
                   pair[prevPairNumb].winner);

            printf("lets see if the %ith loser is a previous winner\n\n", pair[prevPairNumb].loser);

            currentLoser = previousLoser;
            Counter++;
            // run the code again and add one to the counter
            return cycleCheck(pairNumber, pair, currentLoser, currentWinner, Counter);
        }
    }
    printf("no cycle found\n\n");

    return false;
}

//
int winnerCheck(int col, bool arr[][MAX])
{
    for (int row = 0; row < pair_count; row++)
    {
        // recursive case
        if (arr[row][col] == true)
        {
            return winnerCheck(col + 1, arr);
        }
    }
    return col;
}

// Print the winner of the election
void print_winner(void)
{
    int column = 0;

    printf("%s\n", candidates[winnerCheck(column, locked)]);
    return;
}
