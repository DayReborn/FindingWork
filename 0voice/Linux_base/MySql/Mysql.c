#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ZZX_DB_SERVER_IP "192.168.5.128"
#define ZZX_DB_SERVER_PORT 3306

#define ZZX_DB_SERVER_NAME "admin"
#define ZZX_DB_SERVER_PASSWORD "Zzx123456@"
#define ZZX_DB_DEFAULTDB "ZZX_DB"

#define SQL_INSERT_TBL_USER "INSERT TBL_USER(U_NAME, U_GENGDER) VALUE('wxm' , 'woman');"
#define SQL_SELECT_TBL_USER "SELECT * FROM TBL_USER;"

#define SQL_DELETE_TBL_USER "CALL PROC_DELETE_USER('wxm');"

#define SQL_INSERT_IMG_USER "INSERT TBL_USER(U_NAME, U_GENGDER, U_IMG) VALUE('wxm' , 'woman', ?);"

#define SQL_SELECT_IMG_USER "SELECT U_IMG FROM TBL_USER WHERE U_NAME = 'wxm';"

#define FILE_IMAGE_LENGTH (64*1024)



// C U R D
int zzx_mysql_select(MYSQL *mysql)
{
    // 1. 发送查询请求
    if (mysql_real_query(mysql, SQL_SELECT_TBL_USER, strlen(SQL_SELECT_TBL_USER)) != 0)
    {
        printf("mysql_real_query() failed: %s\n", mysql_error(mysql));
        return -1;
    }
    else
    {
        printf("mysql_real_query() success\n");
    }

    // 2. 存储结果集
    MYSQL_RES *result = mysql_store_result(mysql);
    if (result == NULL)
    {
        printf("mysql_store_result() failed: %s\n", mysql_error(mysql));
        return -2;
    }
    else
    {
        printf("mysql_store_result() success\n");
    }

    // 3. 分析有多少行列
    int rows = mysql_num_rows(result);
    printf("rows = %d\n", rows);

    int fields = mysql_num_fields(result);
    printf("fields = %d\n", fields);

    // 4. 遍历结果集
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) != NULL)
    {
        for (int i = 0; i < fields; i++)
        {
            printf("%s ", row[i] ? row[i] : "NULL");
        }
        printf("\n");
    }

    // 5. 释放结果集
    mysql_free_result(result);
    return 0;
}



int read_image(char *filename, char *buffer){
    if (filename == NULL || buffer == NULL)
    {
        printf("Invalid arguments\n");
        return -1;
    }
    
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        printf("Failed to open file: %s\n", filename);
        return -2;
    }
    // file size
    fseek(fp, 0, SEEK_END);
    int length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    // read file
    int read_size = fread(buffer, 1, length, fp);
    if (read_size != length)
    {
        printf("Failed to read file: %s\n", filename);
        fclose(fp);
        return -3;
    }
    fclose(fp);
    printf("Read file: %s, size: %d\n", filename, length);
    return read_size;
}


int write_image(char *filename, char *buffer, int length){
    if(filename == NULL || buffer == NULL || length <= 0){
        printf("Invalid arguments\n");
        return -1;
    }
    FILE *fp = fopen(filename, "wb+");
    if(fp == NULL){
        printf("Failed to open file: %s\n", filename);
        return -2;
    }
    int write_size = fwrite(buffer, 1, length, fp);
    if(write_size != length){
        printf("Failed to write file: %s\n", filename);
        fclose(fp);
        return -3;
    }
    fclose(fp);
    printf("Write file: %s, size: %d\n", filename, length);
    return write_size;
}

int mysql_write_image(MYSQL *handle, char *buffer, int length){
    if (handle == NULL || buffer == NULL || length <= 0){
        printf("Invalid arguments\n");
        return -1;
    }
    
    MYSQL_STMT *stmt = mysql_stmt_init(handle);
    int ret = mysql_stmt_prepare(stmt, SQL_INSERT_IMG_USER, strlen(SQL_INSERT_IMG_USER));
    if(ret != 0){
        printf("mysql_stmt_prepare() failed: %s\n", mysql_error(handle));
        mysql_stmt_close(stmt);
        return -2;
    }

    MYSQL_BIND param = {0};

    param.buffer_type = MYSQL_TYPE_LONG_BLOB;
    param.buffer = NULL;
    param.is_null = 0;
    param.length = NULL;
    ret = mysql_stmt_bind_param(stmt, &param);
    if(ret != 0){
        printf("mysql_stmt_bind_param() failed: %s\n", mysql_error(handle));
        mysql_stmt_close(stmt);
        return -3;
    }

    ret = mysql_stmt_send_long_data(stmt, 0, buffer, length);
    if(ret != 0){
        printf("mysql_stmt_send_long_data() failed: %s\n", mysql_error(handle));
        mysql_stmt_close(stmt);
        return -4;
    }

    ret = mysql_stmt_execute(stmt);
    if(ret != 0){
        printf("mysql_stmt_execute() failed: %s\n", mysql_error(handle));
        mysql_stmt_close(stmt);
        return -5;
    }

    ret = mysql_stmt_close(stmt);
    if(ret != 0){
        printf("mysql_stmt_close() failed: %s\n", mysql_error(handle));
        return -6;
    }

    printf("mysql_write_image() success\n");
    return 0;
}


int mysql_read_image(MYSQL *handle, char *buffer, int length){
    if (handle == NULL || buffer == NULL || length <= 0){
        printf("Invalid arguments\n");
        return -1;
    }
    
    MYSQL_STMT *stmt = mysql_stmt_init(handle);
    int ret = mysql_stmt_prepare(stmt, SQL_SELECT_IMG_USER, strlen(SQL_INSERT_IMG_USER));
    if(ret != 0){
        printf("mysql_stmt_prepare() failed: %s\n", mysql_error(handle));
        mysql_stmt_close(stmt);
        return -2;
    }
    MYSQL_BIND result = {0};
    result.buffer_type = MYSQL_TYPE_LONG_BLOB;
    unsigned long total_length = 0;
    result.length = &total_length;

    ret = mysql_stmt_bind_result(stmt, &result);
    if(ret != 0){
        printf("mysql_stmt_bind_result() failed: %s\n", mysql_error(handle));
        mysql_stmt_close(stmt);
        return -3;
    }

    ret = mysql_stmt_execute(stmt);
    if(ret != 0){
        printf("mysql_stmt_execute() failed: %s\n", mysql_error(handle));
        mysql_stmt_close(stmt);
        return -4;
    }

    ret = mysql_stmt_store_result(stmt);
    if(ret != 0){
        printf("mysql_stmt_store_result() failed: %s\n", mysql_error(handle));
        mysql_stmt_close(stmt);
        return -5;
    }
   
    // 如果只有一项可以不要while
    while(1){
        ret = mysql_stmt_fetch(stmt);
        if(ret != 0 && ret != MYSQL_DATA_TRUNCATED){
            printf("mysql_stmt_fetch() failed: %s\n", mysql_error(handle));
            break;
        }
        int start = 0;
        while(start<(int)total_length){
            result.buffer = buffer + start;
            result.buffer_length = 1;
            mysql_stmt_fetch_column(stmt, &result, 0, start);
            start += result.buffer_length;

        }
    }
    mysql_stmt_free_result(stmt);
    ret = mysql_stmt_close(stmt);
    if(ret != 0){
        printf("mysql_stmt_close() failed: %s\n", mysql_error(handle));
        return -6;
    }
    printf("mysql_read_image() success\n");
    return total_length;


}


int main()
{

    MYSQL mysql;

    if (mysql_init(&mysql) == NULL)
    {
        printf("mysql_init() failed: %s\n", mysql_error(&mysql));
        return -1;
    }

    if (mysql_real_connect(&mysql, ZZX_DB_SERVER_IP,
                           ZZX_DB_SERVER_NAME,
                           ZZX_DB_SERVER_PASSWORD,
                           ZZX_DB_DEFAULTDB,
                           ZZX_DB_SERVER_PORT,
                           NULL,
                           0) == 0)
    {
        printf("mysql_real_connect() failed: %s\n", mysql_error(&mysql));
        return -2;
    }
    else
    {
        printf("mysql_real_connect() success\n");
    }

    // mysql --> select
    if (zzx_mysql_select(&mysql) != 0)
    {
        printf("zzx_mysql_select() failed\n");
        return -3;
    }
    else
    {
        printf("zzx_mysql_select() success\n");
    }

    // mysql --> insert
#if 0
    
    if (mysql_real_query(&mysql, SQL_INSERT_TBL_USER, strlen(SQL_INSERT_TBL_USER)) != 0)
    {
        printf("mysql_real_query() failed: %s\n", mysql_error(&mysql));
        goto Exit;
    }
    else
    {
        printf("mysql_real_query() success\n");
    }
#endif

    zzx_mysql_select(&mysql);
    // mysql --> delete
#if 1
    if (mysql_real_query(&mysql, SQL_DELETE_TBL_USER, strlen(SQL_DELETE_TBL_USER)) != 0)
    {
        printf("mysql_real_query() failed: %s\n", mysql_error(&mysql));
        goto Exit;
    }
    else
    {
        printf("mysql_real_query() success\n");
    }
#endif
    zzx_mysql_select(&mysql);


    // mysql --> insert image
    printf("case: mysql --> insert image\n");
    char buffer[FILE_IMAGE_LENGTH] = {0};
    int length = read_image("test.jpg", buffer);
    if(length < 0){
        printf("read_image() failed\n");
        goto Exit;
    }
    printf("read_image() success\n");

    mysql_write_image(&mysql, buffer, length);

    printf("mysql_write_image() success\n");

    memset(buffer, 0, FILE_IMAGE_LENGTH);
    length = mysql_read_image(&mysql, buffer, FILE_IMAGE_LENGTH);
    write_image("test1.jpg", buffer, length);
    if(length < 0){
        printf("mysql_read_image() failed\n");
        goto Exit;
    }
    printf("mysql_read_image() success\n");

    // mysql --> select image

Exit:
    mysql_close(&mysql);
    return 0;
}