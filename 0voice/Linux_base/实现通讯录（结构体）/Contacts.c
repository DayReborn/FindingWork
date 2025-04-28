// 整体用双向链表来进行存储
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NAME_LENGTH 16
#define PHONE_LENGTH 32
#define BUFFER_LENGTH 80
#define MIN_TOKEN_LENGTH 8

#define INFO printf

#define LIST_INSERT(item, list)    \
    do                             \
    {                              \
        if ((list) != NULL)        \
            (list)->prev = (item); \
        (item)->prev = NULL;       \
        (item)->next = (list);     \
        (list) = (item);           \
    } while (0)

#define LIST_REMOVE(item, list)            \
    do                                     \
    {                                      \
        if (item->prev != NULL)            \
            item->prev->next = item->next; \
        if (item->next != NULL)            \
            item->next->prev = item->prev; \
        if (list == item)                  \
            list = item->next;             \
        item->prev = NULL;                 \
        item->next = NULL;                 \
    } while (0)

/**
 * @brief 个人信息节点
 */
typedef struct Person
{
    char name[NAME_LENGTH];
    char phone[PHONE_LENGTH];
    struct Person *next;
    struct Person *prev;
} person;

/**
 * @brief 通讯录数据结构
 */
typedef struct Contacts
{
    person *people;
    int count;
} contacts;

// TODO：实现数据结构与逻辑实现的分离

enum
{
    OPER_INSERT = 1,
    OPER_PRINT = 2,
    OPER_DELETE = 3,
    OPER_SEARCH = 4,
    OPER_SAVE = 5,
    OPER_LOAD = 6
};

/**
 * @brief 往链表头部插入一个新的联系人节点
 * @param ppeople 链表头指针的地址
 * @param ps 要插入的联系人节点
 * @return 成功返回0，失败返回-1
 */
int person_insert(person **ppeople, person *ps)
{
    if (ps == NULL)
        return -1;
    LIST_INSERT(ps, *ppeople);
    return 0;
}

/**
 * @brief 从链表中删除一个联系人节点
 * @param ppeople 链表头指针的地址
 * @param ps 要删除的联系人节点
 * @return 成功返回0，失败返回-1
 */
int person_delete(person **ppeople, person *ps)
{
    if (ps == NULL)
        return -1;
    LIST_REMOVE(ps, *ppeople);
    return 0;
}

/**
 * @brief 根据名字在联系人链表中查找联系人
 * @param people 链表头指针
 * @param name 要查找的名字
 * @return 找到返回对应节点指针，未找到返回NULL
 */
person *person_search(person *people, const char *name)
{
    while (people != NULL)
    {
        if (strcmp(people->name, name) == 0)
            return people;
        people = people->next;
    }
    return NULL;
}

/**
 * @brief 遍历打印所有联系人
 * @param people 链表头指针
 * @return 成功返回0
 */
int person_traversal(person *people)
{
    while (people != NULL)
    {
        INFO("name:%s, phone:%s\n", people->name, people->phone);
        people = people->next;
    }
    return 0;
}

/**
 * @brief 将联系人链表保存到指定文件
 * @param people 链表头指针
 * @param filename 文件名
 * @return 成功返回0，失败返回-1
 */
int save_file(person *people, const char *filename)
{
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
        return -1;

    while (people != NULL)
    {
        fprintf(fp, "Name: %s,Phone: %s\n", people->name, people->phone);
        fflush(fp); // 刷新文件流,从而将缓冲区的内容写入文件。落盘
        people = people->next;
    }
    fclose(fp);
    return 0;
}

/**
 * @brief 从读取的一行字符串中解析出名字和电话号码
 * @param buffer 读取到的字符串缓冲区
 * @param length 字符串长度
 * @param name 解析出的名字
 * @param phone 解析出的电话号码
 * @return 成功返回0，参数错误返回-1，字符串太短返回-2
 */
int parser_token(char *buffer, int length, char *name, char *phone)
{
    if (buffer == NULL || name == NULL || phone == NULL)
        return -1;
    if (length < MIN_TOKEN_LENGTH) // 8
        return -2;
    int i = 0, j = 0, status = 0;
    for (i = 0; buffer[i] != ','; i++)
    {
        if (buffer[i] == ' ')
        {
            status = 1;
        }
        else if (status == 1)
        {
            name[j++] = buffer[i];
        }
    }
    status = 0;
    j = 0;
    for (i = i + 1; i < BUFFER_LENGTH; i++)
    {
        if (buffer[i] == ' ')
        {
            status = 1;
        }
        else if (status == 1)
        {
            phone[j++] = buffer[i];
        }
    }
    INFO("name:%s, phone:%s\n", name, phone);
    return 0;
}

/**
 * @brief 从文件中加载联系人到链表
 * @param ppeople 链表头指针的地址
 * @param count 联系人数量指针
 * @param filename 文件名
 * @return 成功返回0，文件打开失败返回-1，内存分配失败返回-2
 */
int load_file(person **ppeople, int *count, const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
        return -1;

    while (!feof(fp))
    {
        char buffer[BUFFER_LENGTH] = {0};
        char name[NAME_LENGTH] = {0};
        char phone[PHONE_LENGTH] = {0};

        if (fgets(buffer, BUFFER_LENGTH, fp) == NULL)
            break;

        // -------- 去除行尾的 \r 和 \n，防止多余空行 --------
        buffer[strcspn(buffer, "\r\n")] = '\0';

        if (parser_token(buffer, strlen(buffer), name, phone) != 0)
            continue;

        person *p = (person *)malloc(sizeof(person));
        if (p == NULL)
            return -2;
        memcpy(p->name, name, NAME_LENGTH);
        memcpy(p->phone, phone, PHONE_LENGTH);

        if (person_insert(ppeople, p) != 0)
        {
            INFO("insert person failed\n");
            free(p);
            return -3;
        }
        (*count)++;
    }
    fclose(fp);
    return 0;
}
/**
 * @brief 按照名字的首字母对联系人链表进行排序
 * @param ppeople 链表头指针的地址
 * @return 成功返回0，失败返回-1
 */

int sort_list_by_initial(person **ppeople)
{
    if (ppeople == NULL || *ppeople == NULL)
        return -1;

    person *sorted = NULL;
    person *current = *ppeople;

    while (current != NULL)
    {
        person *next = current->next;
        // 将 current 与原链表断开
        current->prev = current->next = NULL;

        // 在 sorted 链表中找到插入位置
        person *iter = sorted;
        person *prev = NULL;
        char cur_initial = tolower(current->name[0]);
        while (iter != NULL && tolower(iter->name[0]) <= cur_initial)
        {
            prev = iter;
            iter = iter->next;
        }

        if (prev == NULL)
        {
            // 插入到头部
            current->next = sorted;
            if (sorted)
                sorted->prev = current;
            sorted = current;
        }
        else
        {
            // 插入到 prev 之后
            current->next = prev->next;
            if (prev->next)
                prev->next->prev = current;
            prev->next = current;
            current->prev = prev;
        }

        current = next;
    }

    *ppeople = sorted;
    return 0;
}

/**
 * @brief 插入新的联系人
 * @param cts 通讯录指针
 * @return 成功返回0，失败返回负值
 */
int insert_entry(contacts *cts)
{
    if (cts == NULL)
        return -1;

    person *p = (person *)malloc(sizeof(person));
    if (p == NULL)
        return -2;

    // name
    // TODO: Linux下如何解决scanf数组输入溢出的问题
    INFO("Please input name:");
    scanf("%s", p->name);

    // phone
    INFO("Please input phone:");
    scanf("%s", p->phone);

    // add person

    if (0 != person_insert(&cts->people, p))
    {
        INFO("insert person failed\n");
        free(p);
        return -3;
    }

    if (0 != sort_list_by_initial(&cts->people))
    {
        INFO("sort list failed\n");
        free(p);
        return -4;
    }

    // add count
    cts->count++;
    INFO("insert person success\n");
    INFO("name:%s,phone:%s\n", p->name, p->phone);
    return 0;
}

/**
 * @brief 打印所有联系人
 * @param cts 通讯录指针
 * @return 成功返回0
 */
int print_entry(contacts *cts)
{
    if (cts == NULL)
        return -1;

    // print all
    INFO("print all:\n");
    person_traversal(cts->people);
    INFO("total count:%d\n", cts->count);
    INFO("print success\n");
    return 0;
}

/**
 * @brief 删除指定联系人
 * @param cts 通讯录指针
 * @return 成功返回0，失败返回负值
 */
int delete_entry(contacts *cts)
{
    if (cts == NULL)
        return -1;

    char name[NAME_LENGTH];
    INFO("Please input name:");
    scanf("%s", name);

    person *ps = person_search(cts->people, name);
    if (ps == NULL)
    {
        INFO("person not found\n");
        return -2;
    }

    // delete person
    if (0 != person_delete(&cts->people, ps))
    {
        INFO("delete person failed\n");
        return -3;
    }

    // delete count
    cts->count--;
    INFO("delete person success\n");
    free(ps);
    return 0;
}

/**
 * @brief 查找指定联系人
 * @param cts 通讯录指针
 * @return 成功返回0，未找到返回-2
 */
int search_entry(contacts *cts)
{
    if (cts == NULL)
        return -1;

    char name[NAME_LENGTH] = {0};
    INFO("Please input name: ");
    scanf("%s", name);

    person *ps = person_search(cts->people, name);
    if (ps == NULL)
    {
        INFO("person not found\n");
        return -2;
    }

    // print person
    INFO("name:%s, phone:%s\n", ps->name, ps->phone);
    return 0;
}

/**
 * @brief 保存联系人到文件
 * @param cts 通讯录指针
 * @return 成功返回0
 */
int save_entry(contacts *cts)
{
    if (cts == NULL)
        return -1;
    INFO("Please input filename:");
    char filename[BUFFER_LENGTH] = {0};
    scanf("%s", filename);
    save_file(cts->people, filename);
    INFO("save file success\n");
    return 0;
}

/**
 * @brief 从文件中加载联系人
 * @param cts 通讯录指针
 * @return 成功返回0，失败返回负值
 */
int load_entry(contacts *cts)
{
    if (cts == NULL)
        return -1;
    INFO("Please input filename:");
    char filename[BUFFER_LENGTH] = {0};
    scanf("%s", filename);
    load_file(&cts->people, &cts->count, filename);
    if (0 != sort_list_by_initial(&cts->people))
    {
        INFO("sort list failed\n");
        return -2;
    }
    INFO("load file success\n");

    return 0;
}

/**
 * @brief 打印菜单信息
 */
void manu_info()
{
    INFO("\n\n**************************************************\n");
    INFO("***************  Welcome to Contacts  ************\n");
    INFO("***************  1. Insert Person  ***************\n");
    INFO("***************  2. Print Person   ***************\n");
    INFO("***************  3. Delete Person  ***************\n");
    INFO("***************  4. Search Person  ***************\n");
    INFO("***************  5. Save Files     ***************\n");
    INFO("***************  6. Load Files     ***************\n");
    INFO("***************  0. Exit           ***************\n");
    INFO("***************  Please select:    ***************\n");
    INFO("**************************************************\n\n");
}

/**
 * @brief 主函数，程序入口
 * @return 正常退出返回0
 */
int main()
{
    contacts *cts = (contacts *)malloc(sizeof(contacts));
    cts->people = NULL;
    cts->count = 0;
    while (1)
    {
        manu_info();
        int select = 0;
        scanf("%d", &select);
        switch (select)
        {
        case OPER_INSERT:
            insert_entry(cts);
            break;
        case OPER_PRINT:
            print_entry(cts);
            break;
        case OPER_DELETE:
            delete_entry(cts);
            break;
        case OPER_SEARCH:
            search_entry(cts);
            break;
        case OPER_SAVE:
            save_entry(cts);
            break;
        case OPER_LOAD:
            load_entry(cts);
            break;
        default:
            goto exit;
        }
    }
exit:
    free(cts);
    cts = NULL;
    INFO("exit\n");
    return 0;
}
