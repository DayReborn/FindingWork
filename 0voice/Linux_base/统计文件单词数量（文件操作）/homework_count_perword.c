#include <stdio.h>
#include <stdlib.h>

#define OUT 0   // 空闲状态
#define IN 1    // 正在读单词
#define TEMP 2  // 刚读到 '-'，待判定
#define TEMP1 3 // 在 '-' 之后遇到 '\n'，待判定

typedef struct letter_node
{
    int count;
    struct letter_node *children[27];
} letter_node;

letter_node *create_node(void)
{
    letter_node *temp_node = malloc(sizeof(letter_node));
    temp_node->count = 0;
    for (int i = 0; i < 27; ++i)
    {
        temp_node->children[i] = NULL;
    }
    return temp_node;
}

int is_letter(int c)
{
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

int count_word(const char *filename, letter_node *Dummy_node)
{
    int state = OUT;
    FILE *fp = fopen(filename, "r");
    if (!fp)
        return -1;

    int word_num = 0;
    int c;

    letter_node *temp_node;

    while ((c = fgetc(fp)) != EOF)
    {
        switch (state)
        {
        case OUT:
            if (is_letter(c))
            {
                state = IN;
                word_num++;
                c = c >= 'A' && c <= 'Z' ? c - 'A' + 'a' : c;
                if (Dummy_node->children[c - 'a'] == NULL)
                {
                    Dummy_node->children[c - 'a'] = create_node();
                }
                temp_node = Dummy_node->children[c - 'a'];
            }
            // 其他情况都留在 OUT
            break;

        case IN:
            if (is_letter(c))
            {
                c = c >= 'A' && c <= 'Z' ? c - 'A' + 'a' : c;
                if (temp_node->children[c - 'a'] == NULL)
                {
                    temp_node->children[c - 'a'] = create_node();
                }
                temp_node = temp_node->children[c - 'a'];
                // 继续在词中
            }
            else if (c == '-')
            {
                state = TEMP; // 可能的连字符
            }
            else
            {
                temp_node->count++;
                temp_node = NULL;
                state = OUT; // 遇到非字母非'-'，词结束
            }
            break;

        case TEMP:
            if (is_letter(c))
            {
                if (temp_node->children[26] == NULL)
                {
                    temp_node->children[26] = create_node();
                }
                temp_node = temp_node->children[26];

                c = c >= 'A' && c <= 'Z' ? c - 'A' + 'a' : c;

                if (temp_node->children[c - 'a'] == NULL)
                {
                    temp_node->children[c - 'a'] = create_node();
                }

                temp_node = temp_node->children[c - 'a'];

                state = IN; // 段内连字符，如 JNJ-code
            }
            else if (c == '\n')
            {
                state = TEMP1; // 换行连字符，如 acon-\n 后面待判
            }
            else
            {
                temp_node->count++;
                temp_node = NULL;
                state = OUT; // 既不是字母也不是换行，回到 OUT
            }
            break;

        case TEMP1:
            if (is_letter(c))
            {
                c = c >= 'A' && c <= 'Z' ? c - 'A' + 'a' : c;

                if (temp_node->children[c - 'a'] == NULL)
                {
                    temp_node->children[c - 'a'] = create_node();
                }

                temp_node = temp_node->children[c - 'a'];

                state = IN; // 连字符换行后字母，继续 IN
            }
            else
            {
                temp_node->count++;
                temp_node = NULL;
                state = OUT; // 不是字母，视为词已结束
            }
            break;
        }
    }

    if (temp_node != NULL)
        temp_node->count++;
    fclose(fp);
    return word_num;
}

// 前向声明辅助函数
void traverse(letter_node *node, char *word, int depth);

void print_result(letter_node *Dummy_node)
{
    char buffer[1024]; // 假设单词最大长度为1023字符
    for (int i = 0; i < 26; ++i)
    { // 遍历所有可能的首字母（a-z）
        letter_node *child = Dummy_node->children[i];
        if (child != NULL)
        {
            buffer[0] = 'a' + i;        // 设置首字母
            traverse(child, buffer, 1); // 深度从1开始
        }
    }
}

void traverse(letter_node *node, char *word, int depth)
{
    if (node == NULL)
        return;

    // 如果当前节点有计数，输出单词
    if (node->count > 0)
    {
        word[depth] = '\0'; // 终止字符串
        printf("%s: %d\n", word, node->count);
    }

    // 遍历所有子节点（a-z和连字符）
    for (int i = 0; i < 27; ++i)
    {
        if (node->children[i] != NULL)
        {
            char c;
            if (i < 26)
            {
                c = 'a' + i;
            }
            else
            {
                c = '-';
            }
            word[depth] = c; // 添加当前字符到单词中
            traverse(node->children[i], word, depth + 1);
        }
    }
}

int main(int argc, char *argv[])
{
    letter_node *Dummy_node = create_node();
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        return 1;
    }
    int n = count_word(argv[1], Dummy_node);
    if (n < 0)
    {
        perror("fopen");
        return 1;
    }
    printf("nums: %d\n", n);
    printf("==========================\n");
    printf("单词统计如下：\n");
    print_result(Dummy_node);
    printf("==========================\n");
    return 0;
}
