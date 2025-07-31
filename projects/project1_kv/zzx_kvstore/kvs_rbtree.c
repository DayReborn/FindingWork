

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kvstore.h"

rbtree_node *rbtree_mini(rbtree *T, rbtree_node *x)
{
	while (x->left != T->nil)
	{
		x = x->left;
	}
	return x;
}

rbtree_node *rbtree_maxi(rbtree *T, rbtree_node *x)
{
	while (x->right != T->nil)
	{
		x = x->right;
	}
	return x;
}

rbtree_node *rbtree_successor(rbtree *T, rbtree_node *x)
{
	rbtree_node *y = x->parent;

	if (x->right != T->nil)
	{
		return rbtree_mini(T, x->right);
	}

	while ((y != T->nil) && (x == y->right))
	{
		x = y;
		y = y->parent;
	}
	return y;
}

void rbtree_left_rotate(rbtree *T, rbtree_node *x)
{

	rbtree_node *y = x->right; // x  --> y  ,  y --> x,   right --> left,  left --> right

	x->right = y->left; // 1 1
	if (y->left != T->nil)
	{ // 1 2
		y->left->parent = x;
	}

	y->parent = x->parent; // 1 3
	if (x->parent == T->nil)
	{ // 1 4
		T->root = y;
	}
	else if (x == x->parent->left)
	{
		x->parent->left = y;
	}
	else
	{
		x->parent->right = y;
	}

	y->left = x;   // 1 5
	x->parent = y; // 1 6
}

void rbtree_right_rotate(rbtree *T, rbtree_node *y)
{

	rbtree_node *x = y->left;

	y->left = x->right;
	if (x->right != T->nil)
	{
		x->right->parent = y;
	}

	x->parent = y->parent;
	if (y->parent == T->nil)
	{
		T->root = x;
	}
	else if (y == y->parent->right)
	{
		y->parent->right = x;
	}
	else
	{
		y->parent->left = x;
	}

	x->right = y;
	y->parent = x;
}

void rbtree_insert_fixup(rbtree *T, rbtree_node *z)
{

	while (z->parent->color == RED)
	{ // z ---> RED
		if (z->parent == z->parent->parent->left)
		{
			rbtree_node *y = z->parent->parent->right;
			if (y->color == RED)
			{
				z->parent->color = BLACK;
				y->color = BLACK;
				z->parent->parent->color = RED;

				z = z->parent->parent; // z --> RED
			}
			else
			{

				if (z == z->parent->right)
				{
					z = z->parent;
					rbtree_left_rotate(T, z);
				}

				z->parent->color = BLACK;
				z->parent->parent->color = RED;
				rbtree_right_rotate(T, z->parent->parent);
			}
		}
		else
		{
			rbtree_node *y = z->parent->parent->left;
			if (y->color == RED)
			{
				z->parent->color = BLACK;
				y->color = BLACK;
				z->parent->parent->color = RED;

				z = z->parent->parent; // z --> RED
			}
			else
			{
				if (z == z->parent->left)
				{
					z = z->parent;
					rbtree_right_rotate(T, z);
				}

				z->parent->color = BLACK;
				z->parent->parent->color = RED;
				rbtree_left_rotate(T, z->parent->parent);
			}
		}
	}

	T->root->color = BLACK;
}

void rbtree_insert(rbtree *T, rbtree_node *z)
{

	rbtree_node *y = T->nil;
	rbtree_node *x = T->root;

	while (x != T->nil)
	{
		y = x;
#if ENABLE_KEY_CHAR

		if (strcmp(z->key, x->key) < 0)
		{
			x = x->left;
		}
		else if (strcmp(z->key, x->key) > 0)
		{
			x = x->right;
		}
		else
		{
			return;
		}

#else
		if (z->key < x->key)
		{
			x = x->left;
		}
		else if (z->key > x->key)
		{
			x = x->right;
		}
		else
		{ // Exist
			return;
		}
#endif
	}

	z->parent = y;
	if (y == T->nil)
	{
		T->root = z;
#if ENABLE_KEY_CHAR
	}
	else if (strcmp(z->key, y->key) < 0)
	{
#else
	}
	else if (z->key < y->key)
	{
#endif
		y->left = z;
	}
	else
	{
		y->right = z;
	}

	z->left = T->nil;
	z->right = T->nil;
	z->color = RED;

	rbtree_insert_fixup(T, z);
}

void rbtree_delete_fixup(rbtree *T, rbtree_node *x)
{

	while ((x != T->root) && (x->color == BLACK))
	{
		if (x == x->parent->left)
		{

			rbtree_node *w = x->parent->right;
			if (w->color == RED)
			{
				w->color = BLACK;
				x->parent->color = RED;

				rbtree_left_rotate(T, x->parent);
				w = x->parent->right;
			}

			if ((w->left->color == BLACK) && (w->right->color == BLACK))
			{
				w->color = RED;
				x = x->parent;
			}
			else
			{

				if (w->right->color == BLACK)
				{
					w->left->color = BLACK;
					w->color = RED;
					rbtree_right_rotate(T, w);
					w = x->parent->right;
				}

				w->color = x->parent->color;
				x->parent->color = BLACK;
				w->right->color = BLACK;
				rbtree_left_rotate(T, x->parent);

				x = T->root;
			}
		}
		else
		{

			rbtree_node *w = x->parent->left;
			if (w->color == RED)
			{
				w->color = BLACK;
				x->parent->color = RED;
				rbtree_right_rotate(T, x->parent);
				w = x->parent->left;
			}

			if ((w->left->color == BLACK) && (w->right->color == BLACK))
			{
				w->color = RED;
				x = x->parent;
			}
			else
			{

				if (w->left->color == BLACK)
				{
					w->right->color = BLACK;
					w->color = RED;
					rbtree_left_rotate(T, w);
					w = x->parent->left;
				}

				w->color = x->parent->color;
				x->parent->color = BLACK;
				w->left->color = BLACK;
				rbtree_right_rotate(T, x->parent);

				x = T->root;
			}
		}
	}

	x->color = BLACK;
}

rbtree_node *rbtree_delete(rbtree *T, rbtree_node *z)
{

	rbtree_node *y = T->nil;
	rbtree_node *x = T->nil;

	if ((z->left == T->nil) || (z->right == T->nil))
	{
		y = z;
	}
	else
	{
		y = rbtree_successor(T, z);
	}

	if (y->left != T->nil)
	{
		x = y->left;
	}
	else if (y->right != T->nil)
	{
		x = y->right;
	}

	x->parent = y->parent;
	if (y->parent == T->nil)
	{
		T->root = x;
	}
	else if (y == y->parent->left)
	{
		y->parent->left = x;
	}
	else
	{
		y->parent->right = x;
	}

	if (y != z)
	{
#if ENABLE_KEY_CHAR

		void *tmp = z->key;
		z->key = y->key;
		y->key = tmp;

		tmp = z->value;
		z->value = y->value;
		y->value = tmp;

#else
		z->key = y->key;
		z->value = y->value;
#endif
	}

	if (y->color == BLACK)
	{
		rbtree_delete_fixup(T, x);
	}

	return y;
}

rbtree_node *rbtree_search(rbtree *T, KEY_TYPE key)
{

	rbtree_node *node = T->root;
	while (node != T->nil)
	{
#if ENABLE_KEY_CHAR

		if (strcmp(key, node->key) < 0)
		{
			node = node->left;
		}
		else if (strcmp(key, node->key) > 0)
		{
			node = node->right;
		}
		else
		{
			return node;
		}

#else
		if (key < node->key)
		{
			node = node->left;
		}
		else if (key > node->key)
		{
			node = node->right;
		}
		else
		{
			return node;
		}
#endif
	}
	return T->nil;
}

void rbtree_traversal(rbtree *T, rbtree_node *node)
{
	if (node != T->nil)
	{
		rbtree_traversal(T, node->left);
#if ENABLE_KEY_CHAR
		printf("key:%s, value:%s\n", node->key, (char *)node->value);
#else
		printf("key:%d, color:%d\n", node->key, node->color);
#endif
		rbtree_traversal(T, node->right);
	}
}

//===================================================
// KVS红黑树相关定义
//===================================================
typedef struct _rbtree kvs_rbtree_t;

kvs_rbtree_t global_rbtree;

/**
 * @brief 创建KVS红黑树实例
 * @param inst 指向kvs_rbtree_t实例的指针
 * @return int 成功返回0，失败返回负数
 * @note 如果inst为NULL，则返回-1
 * @note 如果实例已经创建，则返回-2
 * @note 如果内存分配失败，则返回-3
 *
 */
int kvs_rbtree_create(kvs_rbtree_t *inst)
{

	if (!inst)
	{
		DEBUG("Invalid instance\n");
		return -1;
	}
	if (inst->root || inst->nil)
	{
		DEBUG("Instance already created\n");
		return -2;
	}
	inst->nil = (rbtree_node *)kvs_malloc(sizeof(rbtree_node));
	if (!inst->nil)
	{
		DEBUG("Failed to allocate memory for nil node\n");
		return -3;
	}
	memset(inst->nil, 0, sizeof(rbtree_node));
	inst->nil->color = BLACK; // nil节点颜色为黑色
	inst->root = inst->nil;	  // 初始化根节点为nil
	DEBUG("KVS rbtree created successfully\n");
	return 0; // 成功创建
}

/**
 * @brief 销毁KVS红黑树实例
 * @param inst 指向kvs_rbtree_t实例的指针
 * @note 如果inst为NULL，则不执行任何操作
 */
void kvs_rbtree_destory(kvs_rbtree_t *inst)
{

	if (!inst)
	{
		DEBUG("Invalid instance\n");
		return;
	}
	rbtree_node *node = NULL;

	while (!(node = inst->root))
	{

		rbtree_node *mini = rbtree_mini(inst, node);

		rbtree_node *cur = rbtree_delete(inst, mini);
		kvs_free(cur);
	}
	kvs_free(inst->nil);
	DEBUG("KVS rbtree destroyed successfully\n");
	return;
}


/**
 * @brief 设置键值对到KVS红黑树
 * @param inst 指向kvs_rbtree_t实例的指针
 * @param key 键字符串
 * @param value 值字符串
 * @return 设置成功返回0；键已存在返回正数；失败返回负数
 * @note 如果inst或key/value为NULL，则返回-1
 * @note 如果节点内存分配失败，则返回-2
 * @note 如果key内存分配失败，则返回-3
 * @note 如果value内存分配失败，则返回-4
 * @note 如果键不存在，则添加新的键值对并返回0
 * @note 如果键已存在且值相同，则返回1
 * @note 如果键已存在但值不同，则更新值并返回2
 */
int kvs_rbtree_set(kvs_rbtree_t *inst, char *key, char *value)
{
    // 参数检查
    if (!inst || !key || !value)
    {
        DEBUG("Invalid arguments to kvs_rbtree_set\n");
        return -1;
    }

    // 检查键是否已存在
    rbtree_node *existing = rbtree_search(inst, key);
    if (existing != inst->nil)
    {
        DEBUG("Key already exists: %s\n", key);
        // 检查值是否相同
        if (strcmp(existing->value, value) == 0)
        {
            DEBUG("Value is the same, no need to update\n");
            return 1; // 键已存在且值相同
        }

        // 更新值
        kvs_free(existing->value);
        existing->value = kvs_malloc(strlen(value) + 1);
        if (!existing->value)
        {
            DEBUG("Failed to allocate memory for value copy\n");
            return -4;
        }
        memset(existing->value, 0, strlen(value) + 1);
        strcpy(existing->value, value);
        DEBUG("Updated key: %s with new value: %s\n", key, value);
        return 0; // 键已存在但值不同，已更新
    }

    // 创建新节点
    rbtree_node *newNode = (rbtree_node *)kvs_malloc(sizeof(rbtree_node));
    if (!newNode)
    {
        DEBUG("Failed to allocate memory for new node\n");
        return -2;
    }

    // 分配并复制键
    newNode->key = kvs_malloc(strlen(key) + 1);
    if (!newNode->key)
    {
        DEBUG("Failed to allocate memory for key copy\n");
        kvs_free(newNode);
        return -3;
    }
    memset(newNode->key, 0, strlen(key) + 1);
    strcpy(newNode->key, key);

    // 分配并复制值
    newNode->value = kvs_malloc(strlen(value) + 1);
    if (!newNode->value)
    {
        DEBUG("Failed to allocate memory for value copy\n");
        kvs_free(newNode->key);
        kvs_free(newNode);
        return -4;
    }
    memset(newNode->value, 0, strlen(value) + 1);
    strcpy(newNode->value, value);

    // 插入新节点到红黑树
    rbtree_insert(inst, newNode);
    DEBUG("Added key: %s, value: %s\n", key, value);
    return 0; // 成功添加新键值对
}

/**
 * @brief 检查KVS红黑树中是否存在指定键
 * @param inst 指向kvs_rbtree_t实例的指针
 * @param key 键字符串
 * @return 返回找到的值，如果未找到则返回NULL
 * @note 如果inst或key为NULL，则返回NULL
 * @note 如果键不存在，则返回NULL
 * @note 如果找到键，则返回对应的值指针
 */
char *kvs_rbtree_get(kvs_rbtree_t *inst, char *key)
{
	if (!inst || !key)
	{
		DEBUG("Invalid arguments to kvs_rbtree_get\n");
		return NULL;
	}

	rbtree_node *node = rbtree_search(inst, key);
	if (!node || node == inst->nil)
	{
		DEBUG("Key not found: %s\n", key);
		return NULL; // no exist
	}
	DEBUG("Found key: %s, value: %s\n", node->key, (char *)node->value);
	return node->value; // 返回找到的值
}

/**
 * @brief 删除KVS红黑树中的指定键
 * @param inst 指向kvs_rbtree_t实例的指针
 * @param key 键字符串
 * @return 成功返回0，未找到键返回1，失败返回负数
 * @note 如果inst或key为NULL，则返回-1
 * @note 如果键不存在，则返回1
 * @note 如果成功删除键值对，则返回0
 */
int kvs_rbtree_del(kvs_rbtree_t *inst, char *key)
{
	if (!inst || !key)
	{
		DEBUG("Invalid arguments to kvs_rbtree_del\n");
		return -1;
	}

	rbtree_node *node = rbtree_search(inst, key);
	if (!node || node == inst->nil)
	{
		DEBUG("Key not found for deletion: %s\n", key);
		return 1; // no exist
	}

	rbtree_node *deletedNode = rbtree_delete(inst, node);
	if (!deletedNode)
	{
		DEBUG("Failed to delete node for key: %s\n", key);
		return -2; // 删除失败
	}

	kvs_free(deletedNode->key);
	kvs_free(deletedNode->value);
	kvs_free(deletedNode);

	DEBUG("Successfully deleted key: %s\n", key);
	return 0; // 成功删除
}

/**
 * @brief 修改KVS红黑树中指定键的值
 * @param inst 指向kvs_rbtree_t实例的指针
 * @param key 键字符串
 * @param value 新的值字符串
 * @return 成功返回0，未找到键返回1，失败返回负数
 * @note 如果inst或key/value为NULL，则返回-1
 * @note 如果键不存在，则返回1
 * @note 如果成功修改键值对，则返回0
 */
int kvs_rbtree_mod(kvs_rbtree_t *inst, char *key, char *value)
{
	if (!inst || !key || !value)
	{
		DEBUG("Invalid arguments to kvs_rbtree_mod\n");
		return -1;
	}

	rbtree_node *node = rbtree_search(inst, key);
	if (!node || node == inst->nil)
	{
		DEBUG("Key not found for modification: %s\n", key);
		return 1; // 未找到键
	}

	DEBUG("Modifying key: %s with new value: %s\n", key, value);
	kvs_free(node->value); // 释放旧值
	node->value = kvs_malloc(strlen(value) + 1);
	if (!node->value)
	{
		DEBUG("Failed to allocate memory for new value\n");
		return -2; // 内存分配失败
	}
	memset(node->value, 0, strlen(value) + 1);
	strcpy(node->value, value);

	DEBUG("Successfully modified key: %s with new value: %s\n", key, value);
	return 0; // 成功修改1
}

/**
 * @brief 检查KVS红黑树中是否存在指定键
 * @param inst 指向kvs_rbtree_t实例的指针
 * @param key 键字符串
 * @return 如果键存在返回0，不存在返回1，失败返回-1
 * @note 如果inst或key为NULL，则返回-1
 * @note 如果键不存在，则返回1
 * @note 如果找到键，则返回0
 */
int kvs_rbtree_exist(kvs_rbtree_t *inst, char *key)
{
	if (!inst || !key)
	{
		DEBUG("Invalid arguments to kvs_rbtree_exist\n");
		return -1;
	}

	rbtree_node *node = rbtree_search(inst, key);
	if (!node || node == inst->nil)
	{
		DEBUG("Key does not exist: %s\n", key);
		return 1; // 键不存在
	}

	DEBUG("Key exists: %s\n", key);
	return 0; // 键存在
}




#if 0

int main() {

#if ENABLE_KEY_CHAR

	char* keyArray[10] = {"King", "Darren", "Mark", "Vico", "Nick", "qiuxiang", "youzi", "taozi", "123", "234"};
	char* valueArray[10] = {"1King", "2Darren", "3Mark", "4Vico", "5Nick", "6qiuxiang", "7youzi", "8taozi", "9123", "10234"};

	rbtree *T = (rbtree *)malloc(sizeof(rbtree));
	if (T == NULL) {
		printf("malloc failed\n");
		return -1;
	}
	
	T->nil = (rbtree_node*)malloc(sizeof(rbtree_node));
	T->nil->color = BLACK;
	T->root = T->nil;

	rbtree_node *node = T->nil;
	int i = 0;
	for (i = 0;i < 10;i ++) {
		node = (rbtree_node*)malloc(sizeof(rbtree_node));
		
		node->key = malloc(strlen(keyArray[i]) + 1);
		memset(node->key, 0, strlen(keyArray[i]) + 1);
		strcpy(node->key, keyArray[i]);
		
		node->value = malloc(strlen(valueArray[i]) + 1);
		memset(node->value, 0, strlen(valueArray[i]) + 1);
		strcpy(node->value, valueArray[i]);

		rbtree_insert(T, node);
		
	}

	rbtree_traversal(T, T->root);
	printf("----------------------------------------\n");

	for (i = 0;i < 10;i ++) {

		rbtree_node *node = rbtree_search(T, keyArray[i]);
		rbtree_node *cur = rbtree_delete(T, node);
		free(cur);

		rbtree_traversal(T, T->root);
		printf("----------------------------------------\n");
	}

#else


	int keyArray[20] = {24,25,13,35,23, 26,67,47,38,98, 20,19,17,49,12, 21,9,18,14,15};

	rbtree *T = (rbtree *)malloc(sizeof(rbtree));
	if (T == NULL) {
		printf("malloc failed\n");
		return -1;
	}
	
	T->nil = (rbtree_node*)malloc(sizeof(rbtree_node));
	T->nil->color = BLACK;
	T->root = T->nil;

	rbtree_node *node = T->nil;
	int i = 0;
	for (i = 0;i < 20;i ++) {
		node = (rbtree_node*)malloc(sizeof(rbtree_node));
		node->key = keyArray[i];
		node->value = NULL;

		rbtree_insert(T, node);
		
	}

	rbtree_traversal(T, T->root);
	printf("----------------------------------------\n");

	for (i = 0;i < 20;i ++) {

		rbtree_node *node = rbtree_search(T, keyArray[i]);
		rbtree_node *cur = rbtree_delete(T, node);
		free(cur);

		rbtree_traversal(T, T->root);
		printf("----------------------------------------\n");
	}
#endif

	
}

#endif