#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>   // for getch()
#include <windows.h> // for Sleep()
#include <time.h>

#define MAX_QUES_LEN 300
#define MAX_OPTION_LEN 150

const char *RED = "\033[1;31m";
const char *GREEN = "\033[1;32m";
const char *YELLOW = "\033[1;33m";
const char *BLUE = "\033[1;34m";
const char *PINK = "\033[1;35m";
const char *AQUA = "\033[1;36m";
const char *END = "\033[0m";

typedef struct
{
    char text[MAX_QUES_LEN];
    char options[4][MAX_OPTION_LEN];
    char correct_option;
    int timeout;
    int prize_money;
} Question;

int read_questions(char *file_name, Question **questions);
void print_formatted_question(Question q);
int use_lifeline(Question *q, int *lifeline);

// Windows based timer input function
char timed_input(int seconds)
{
    while (seconds > 0)
    {
        if (kbhit())
        {
            return toupper(getch());
        }
        printf("\rTime Left: %d sec ", seconds);
        Sleep(1000);
        seconds--;
    }
    return 0; // timeout
}

void play_game(Question *questions, int count)
{
    int money_won = 0;
    int lifeline[] = {1, 1};

    for (int i = 0; i < count; i++)
    {
        print_formatted_question(questions[i]);

        printf("\nYour Answer: ");
        char ch = timed_input(questions[i].timeout);

        if (ch == 0)
        {
            printf("\n%sTIME OUT!%s\n", RED, END);
            break;
        }

        printf("%c", ch);

        if (ch == 'L')
        {
            int val = use_lifeline(&questions[i], lifeline);
            if (val != 2)
                i--;
            continue;
        }

        if (ch == questions[i].correct_option)
        {
            printf("\n%sCorrect!%s\n", GREEN, END);
            money_won = questions[i].prize_money;
            printf("You won: Rs %d\n", money_won);
        }
        else
        {
            printf("\n%sWrong! Correct Answer: %c%s\n", RED, questions[i].correct_option, END);
            break;
        }
    }

    printf("\n%sGame Over! Your Total Winnings: Rs %d%s\n", BLUE, money_won, END);
}

int use_lifeline(Question *q, int *lifeline)
{
    printf("\n\nLIFELINES:");
    if (lifeline[0])
        printf("\n1. 50-50");
    if (lifeline[1])
        printf("\n2. Skip Question");
    printf("\nChoose: ");

    char ch = getch();
    printf("%c", ch);

    switch (ch)
    {
    case '1':
        if (lifeline[0])
        {
            lifeline[0] = 0;
            int removed = 0;
            while (removed < 2)
            {
                int x = rand() % 4;
                if ((x + 'A') != q->correct_option && q->options[x][0] != '\0')
                {
                    q->options[x][0] = '\0';
                    removed++;
                }
            }
            return 1;
        }
        break;

    case '2':
        if (lifeline[1])
        {
            lifeline[1] = 0;
            return 2;
        }
        break;
    }

    printf("\nReturning...\n");
    return 0;
}

void print_formatted_question(Question q)
{
    printf("\n\n%s%s%s\n", YELLOW, q.text, END);
    for (int i = 0; i < 4; i++)
        if (q.options[i][0] != '\0')
            printf("%c. %s\n", 'A' + i, q.options[i]);

    printf("%sYou Have %d Seconds!%s\n", YELLOW, q.timeout, END);
}

int read_questions(char *file_name, Question **questions)
{
    FILE *file = fopen(file_name, "r");
    if (!file)
    {
        printf("Could not open questions.txt\n");
        exit(0);
    }

    char temp[MAX_QUES_LEN];
    int lines = 0;

    while (fgets(temp, MAX_QUES_LEN, file))
        lines++;

    int count = lines / 8;
    *questions = (Question *)malloc(count * sizeof(Question));

    rewind(file);

    for (int i = 0; i < count; i++)
    {
        fgets((*questions)[i].text, MAX_QUES_LEN, file);
        for (int j = 0; j < 4; j++)
            fgets((*questions)[i].options[j], MAX_OPTION_LEN, file);

        char buf[20];
        fgets(buf, 20, file);
        (*questions)[i].correct_option = buf[0];

        fgets(buf, 20, file);
        (*questions)[i].timeout = atoi(buf);

        fgets(buf, 20, file);
        (*questions)[i].prize_money = atoi(buf);
    }

    fclose(file);
    return count;
}

int main()
{
    srand(time(NULL));

    Question *questions;
    int count = read_questions("questions.txt", &questions);

    printf("\n\t%sWELCOME TO KAUN BANEGA CROREPATI!!%s\n", PINK, END);

    play_game(questions, count);

    free(questions);
    return 0;
}
