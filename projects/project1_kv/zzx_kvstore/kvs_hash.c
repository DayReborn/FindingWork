#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "kvstore.h"

// Connection
//  'C' + 'o' + 'n'
static int _hash(char *key, int size)
{

	if (!key)
		return -1;

	int sum = 0;
	int i = 0;

	while (key[i] != 0)
	{
		sum += key[i];
		i++;
	}

	return sum % size;
}

hashnode_t *_create_node(char *key, char *value)
{

	hashnode_t *node = (hashnode_t *)kvs_malloc(sizeof(hashnode_t));
	if (!node)
		return NULL;

#if ENABLE_KEY_POINTER
	char *kcopy = kvs_malloc(strlen(key) + 1);
	if (kcopy == NULL)
		return NULL;
	memset(kcopy, 0, strlen(key) + 1);
	strncpy(kcopy, key, strlen(key));

	node->key = kcopy;

	char *kvalue = kvs_malloc(strlen(value) + 1);
	if (kvalue == NULL)
	{
		kvs_free(kvalue);
		return NULL;
	}
	memset(kvalue, 0, strlen(value) + 1);
	strncpy(kvalue, value, strlen(value));

	node->value = kvalue;

#else
	strncpy(node->key, key, MAX_KEY_LEN);
	strncpy(node->value, value, MAX_VALUE_LEN);
#endif
	node->next = NULL;

	return node;
}


kvs_hash_t global_hash = {0};

/**
 * @brief 创建KVS哈希表实例
 * @param hash 指向kvs_hash_t实例的指针
 * @return 成功返回0，失败返回负数
 * @note 如果hash为NULL，则返回-1
 * @note 如果成功创建哈希表实例，则返回0
 * @note 如果哈希表实例已存在，则返回-2
 * @note 如果内存分配失败，则返回-3
 */
int kvs_hash_create(kvs_hash_t *hash)
{
	if (!hash)
	{
		DEBUG("Invalid instance\n");
		return -1;
	}
	if (hash->nodes)
	{
		DEBUG("Instance already created\n");
		return -2;
	}
	hash->nodes = (hashnode_t **)kvs_malloc(sizeof(hashnode_t *) * MAX_TABLE_SIZE);
	if (!hash->nodes)
	{
		DEBUG("Failed to allocate memory for hash nodes\n");
		return -3;
	}
	hash->max_slots = MAX_TABLE_SIZE;
	hash->count = 0;
	DEBUG("KVS hash table created with max slots: %d\n", hash->max_slots);
	return 0; // 成功创建哈希表实例
}

/**
 * @brief 销毁KVS哈希表实例
 * @param hash 指向kvs_hash_t实例的指针
 * @note 如果hash为NULL，则不执行任何操作
 */
void kvs_hash_destory(kvs_hash_t *hash)
{
	if (!hash)
	{
		DEBUG("Invalid instance\n");
		return;
	}
	if (!hash->nodes)
	{
		DEBUG("Hash table nodes already freed\n");
		return;
	}
	
	int i = 0;
	for (i = 0; i < hash->max_slots; i++)
	{
		hashnode_t *node = hash->nodes[i];
		while (node != NULL)
		{
			hashnode_t *tmp = node;
			node = node->next;
			hash->nodes[i] = node;
			
			kvs_free(tmp);
		}

	}
	kvs_free(hash->nodes);
	DEBUG("KVS hash table destroyed\n");
}

/**
 * @brief 设置键值对到KVS哈希表
 * @param hash 指向kvs_hash_t实例的指针
 * @param key 键字符串
 * @param value 值字符串
 * @return 成功返回0，键已存在返回1，失败返回负数
 * @note 如果hash或key/value为NULL，则返回-1
 * @note 如果哈希表已满，则返回-2
 * @note 如果key内存分配失败，则返回-3
 * @note 如果value内存分配失败，则返回-4
 * @note 如果键不存在，则添加新的键值对并返回0
 * @note 如果键已存在且值相同，则返回1
 * @note 如果键已存在但值不同，则更新值并返回0
 */
int kvs_hash_set(kvs_hash_t *hash, char *key, char *value)
{
	// 参数检查
	if (!hash || !key || !value)
	{
		DEBUG("Invalid arguments to kvs_hash_set\n");
		return -1;
	}

	// 检查哈希表是否已满
	if (hash->count >= hash->max_slots)
	{
		DEBUG("Hash table is full\n");
		return -2;
	}

	// 计算哈希值
	int idx = _hash(key, hash->max_slots);
	if (idx < 0)
	{
		DEBUG("Hash calculation failed\n");
		return -1;
	}

	// 检查键是否已存在
	hashnode_t *node = hash->nodes[idx];
	while (node != NULL)
	{
		if (strcmp(node->key, key) == 0)
		{
			// 键存在，检查值是否相同
			if (strcmp(node->value, value) == 0)
			{
				DEBUG("Key exists with same value: %s\n", key);
				return 1; // 键已存在且值相同
			}

// 键存在但值不同，更新值
#if ENABLE_KEY_POINTER
			char *kvalue = kvs_malloc(strlen(value) + 1);
			if (kvalue == NULL)
			{
				DEBUG("Failed to allocate memory for value copy\n");
				return -4;
			}
			memset(kvalue, 0, strlen(value) + 1);
			strncpy(kvalue, value, strlen(value));
			kvs_free(node->value);
			node->value = kvalue;
#else
			strncpy(node->value, value, MAX_VALUE_LEN);
#endif
			DEBUG("Updated key: %s with new value: %s\n", key, value);
			return 0;
		}
		node = node->next;
	}

	// 创建新节点
	hashnode_t *new_node = _create_node(key, value);
	if (new_node == NULL)
	{
		DEBUG("Failed to create new node\n");
		return -3;
	}

	// 插入新节点到链表头部
	new_node->next = hash->nodes[idx];
	hash->nodes[idx] = new_node;
	hash->count++;

	DEBUG("Added new key-value pair: %s -> %s\n", key, value);
	return 0;
}

/**
 * @brief 获取KVS哈希表中的值
 * @param hash 指向kvs_hash_t实例的指针
 * @param key 键字符串
 * @return 成功返回值字符串，未找到键返回NULL，失败返回NULL
 * @note 如果hash或key为NULL，则返回NULL
 */
char *kvs_hash_get(kvs_hash_t *hash, char *key)
{
	if (!hash || !key)
	{
		DEBUG("Invalid arguments to kvs_hash_get\n");
		return NULL;
	}

	// 计算哈希值
	int idx = _hash(key, hash->max_slots);
	if (idx < 0)
	{
		DEBUG("Hash calculation failed\n");
		return NULL;
	}

	// 查找键对应的节点
	hashnode_t *node = hash->nodes[idx];
	while (node != NULL)
	{
		if (strcmp(node->key, key) == 0)
		{
			DEBUG("Found key: %s with value: %s\n", key, node->value);
			return node->value; // 返回找到的值
		}
		node = node->next;
	}

	DEBUG("Key not found: %s\n", key);
	return NULL; // 未找到键
}

/**
 * @brief 修改KVS哈希表中指定键的值
 * @param hash 指向kvs_hash_t实例的指针
 * @param key 键字符串
 * @param value 新的值字符串
 * @return 成功返回0，未找到键返回1，失败返回负数
 * @note 如果hash或key/value为NULL，则返回-1
 * @note 如果键不存在，则返回1
 * @note 如果成功修改键值对，则返回0
 */
int kvs_hash_mod(kvs_hash_t *hash, char *key, char *value)
{

	if (!hash || !key || !value)
	{
		DEBUG("Invalid arguments to kvs_hash_mod\n");
		return -1;
	}

	// 计算哈希值
	int idx = _hash(key, hash->max_slots);
	if (idx < 0)
	{
		DEBUG("Hash calculation failed\n");
		return -1;
	}

	// 查找键对应的节点
	hashnode_t *node = hash->nodes[idx];
	while (node != NULL)
	{
		if (strcmp(node->key, key) == 0)
		{
			// 键存在，更新值
#if ENABLE_KEY_POINTER
			char *kvalue = kvs_malloc(strlen(value) + 1);
			if (kvalue == NULL)
			{
				DEBUG("Failed to allocate memory for value copy\n");
				return -4;
			}
			memset(kvalue, 0, strlen(value) + 1);
			strncpy(kvalue, value, strlen(value));
			kvs_free(node->value);
			node->value = kvalue;
#else
			strncpy(node->value, value, MAX_VALUE_LEN);
#endif
			DEBUG("Modified key: %s with new value: %s\n", key, value);
			return 0; // 成功修改
		}
		// 继续查找下一个节点
		node = node->next;
	}
	DEBUG("Key not found for modification: %s\n", key);
	return 1; // 未找到键
}

int kvs_hash_count(kvs_hash_t *hash)
{
	return hash->count;
}

/**
 * @brief 删除KVS哈希表中的键
 * @param hash 指向kvs_hash_t实例的指针
 * @param key 键字符串
 * @return 成功返回0，未找到键返回1，失败返回负数
 * @note 如果hash或key为NULL，则返回-2
 * @note 如果键不存在，则返回1
 * @note 如果成功删除键值对，则返回0
 */
int kvs_hash_del(kvs_hash_t *hash, char *key)
{
	if (!hash || !key)
	{
		DEBUG("Invalid arguments to kvs_hash_del\n");
		return -2;
	}

	int idx = _hash(key, hash->max_slots);

	hashnode_t *head = hash->nodes[idx];
	if (head == NULL)
	{
		DEBUG("Key not found for deletion: %s\n", key);
		return 1; // noexist
	}
	// head node
	if (strcmp(head->key, key) == 0)
	{
		hashnode_t *tmp = head->next;
		hash->nodes[idx] = tmp;

		kvs_free(head);
		hash->count--;
		DEBUG("Deleted key: %s from hash table\n", key);
		return 0;
	}

	hashnode_t *cur = head;
	while (cur->next != NULL)
	{
		if (strcmp(cur->next->key, key) == 0)
			break; // search node
		// 继续查找下一个节点
		cur = cur->next;
	}

	if (cur->next == NULL)
	{
		DEBUG("Key not found for deletion: %s\n", key);
		return 1; // 未找到键
	}

	hashnode_t *tmp = cur->next;
	cur->next = tmp->next;
	DEBUG("Deleted key: %s from hash table\n", key);
#if ENABLE_KEY_POINTER
	kvs_free(tmp->key);
	kvs_free(tmp->value);
#endif
	kvs_free(tmp);
	// 释放节点内存
	// 减少计数
	hash->count--;

	return 0;
}

/**
 * @brief 检查KVS哈希表中是否存在指定键
 * @param hash 指向kvs_hash_t实例的指针
 * @param key 键字符串
 * @return 如果键存在返回0，不存在返回1，失败返回-1
 * @note 如果hash或key为NULL，则返回-1
 */
int kvs_hash_exist(kvs_hash_t *hash, char *key)
{
	if (!hash || !key)
	{
		DEBUG("Invalid arguments to kvs_hash_exist\n");
		return -1;
	}
	char *value = kvs_hash_get(hash, key);
	if (!value)
	{
		DEBUG("Key does not exist: %s\n", key);
		return 1; // 键不存在
	}

	DEBUG("Key exists: %s with value: %s\n", key, value);
	return 0; // 键存在
}

#if 0
int main() {
	hashtable_t hash = {0}; // 初始化哈希表
	kvs_hash_create(&hash);

	kvs_hash_set(&hash, "Teacher1", "King");
	kvs_hash_set(&hash, "Teacher2", "Darren");
	kvs_hash_set(&hash, "Teacher3", "Mark");
	kvs_hash_set(&hash, "Teacher4", "Vico");
	kvs_hash_set(&hash, "Teacher5", "Nick");

	char *value1 = kvs_hash_get(&hash, "Teacher1");
	printf("Teacher1 : %s\n", value1);

	int ret = kvs_hash_mod(&hash, "Teacher1", "King1");
	printf("mode Teacher1 ret : %d\n", ret);
	
	char *value2 = kvs_hash_get(&hash, "Teacher2");
	printf("Teacher2 : %s\n", value2);

	ret = kvs_hash_del(&hash, "Teacher1");
	printf("delete Teacher1 ret : %d\n", ret);

	ret = kvs_hash_exist(&hash, "Teacher1");
	printf("Exist Teacher1 ret : %d\n", ret);

	kvs_hash_destory(&hash);

	return 0;
}

#endif
