#include <stdio.h>

#define OUT 0   // 空闲状态
#define IN 1    // 正在读单词
#define TEMP 2  // 刚读到 '-'，待判定
#define TEMP1 3 // 在 '-' 之后遇到 '\n'，待判定

int is_letter(int c)
{
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

int count_word(const char *filename)
{
    int state = OUT;
    FILE *fp = fopen(filename, "r");
    if (!fp)
        return -1;

    int word_num = 0;
    int c;
    while ((c = fgetc(fp)) != EOF)
    {
        switch (state)
        {
        case OUT:
            if (is_letter(c))
            {
                state = IN;
                word_num++;
            }
            // 其他情况都留在 OUT
            break;

        case IN:
            if (is_letter(c))
            {
                // 继续在词中
            }
            else if (c == '-')
            {
                state = TEMP; // 可能的连字符
            }
            else
            {
                state = OUT; // 遇到非字母非'-'，词结束
            }
            break;

        case TEMP:
            if (is_letter(c))
            {
                state = IN; // 段内连字符，如 JNJ-code
            }
            else if (c == '\n')
            {
                state = TEMP1; // 换行连字符，如 acon-\n 后面待判
            }
            else
            {
                state = OUT; // 既不是字母也不是换行，回到 OUT
            }
            break;

        case TEMP1:
            if (is_letter(c))
            {
                state = IN; // 连字符换行后字母，继续 IN
            }
            else
            {
                state = OUT; // 不是字母，视为词已结束
            }
            break;
        }
    }

    fclose(fp);
    return word_num;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        return 1;
    }
    int n = count_word(argv[1]);
    if (n < 0)
    {
        perror("fopen");
        return 1;
    }
    printf("nums: %d\n", n);
    return 0;
}
