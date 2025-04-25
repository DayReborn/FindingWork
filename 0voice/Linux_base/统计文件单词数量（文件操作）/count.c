#include <stdio.h>

#define OUT 0
#define IN 1

#define INIT OUT

int count_word(char *filename)
{
    int status = INIT;
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
        return -1;

    int word_num = 0;
    int c;
    while ((c = fgetc(fp)) != EOF)
    {
        if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'))
        {
            if (status == OUT)
            {
                status = IN;
                word_num++;
            }
        }
        else
        {
            if (status == IN)
            {
                status = OUT;
            }
        }
    }
    fclose(fp);
    return word_num;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
        return -1;
    printf("nums: %d", count_word(argv[1]));
}