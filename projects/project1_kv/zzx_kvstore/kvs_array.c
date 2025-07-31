

#include "kvstore.h"

//===================================================
// KVS协议相关定义
//===================================================

/**
 * @brief 分配内存并返回指向分配内存的指针
 * @param size 要分配的内存大小（字节数）
 * @return 返回指向分配内存的指针，如果分配失败则返回NULL
 * @note 使用后请调用kvs_free释放内存
 */
void *kvs_malloc(size_t size)
{
	void *ptr = malloc(size);
	if (!ptr)
	{
		DEBUG("Memory allocation failed\n");
		return NULL;
	}
	return ptr;
}

/**
 * @brief 释放之前分配的内存
 * @param ptr 要释放的内存指针
 * @note 如果ptr为NULL，则不执行任何操作
 */
void kvs_free(void *ptr)
{
	if (ptr)
	{
		free(ptr);
		DEBUG("Memory freed\n");
	}
}

// 使用单例模式

kvs_array_t global_array = {0};

/**
 * @brief 创建KVS数组实例
 * @param inst 指向kvs_array_t实例的指针
 * @return 成功返回0，失败返回负数
 * @note 如果inst或其table成员为NULL，则返回-1
 * @note 如果table已分配内存，则返回-2
 * @note 如果内存分配失败，则返回-3
 */
int kvs_array_create(kvs_array_t *inst)
{
	if (!inst)
	{
		DEBUG("Invalid instance\n");
		return -1;
	}
	if (inst->table)
	{
		DEBUG("Instance already created\n");
		return -2;
	}
	inst->table = kvs_malloc(KVS_ARRAY_SIZE * sizeof(kvs_array_item_t));
	if (!inst->table)
	{
		DEBUG("Failed to allocate memory for kvs_array_item_t\n");
		return -3;
	}
	inst->total = 0;
	inst->idx = 0;
	DEBUG("KVS array created with size: %d\n", KVS_ARRAY_SIZE);
	return 0;
}

/**
 * @brief 销毁KVS数组实例
 * @param inst 指向kvs_array_t实例的指针
 * @note 如果inst为NULL，则不执行任何操作
 */
void kvs_array_destory(kvs_array_t *inst)
{
	if (!inst)
	{
		DEBUG("Invalid instance\n");
		return;
	}
	if (inst->table)
	{
		kvs_free(inst->table);
		DEBUG("kvs_array destroyed\n");
	}
}

/**
 * @brief 设置键值对到KVS数组
 * @param inst 指向kvs_array_t实例的指针
 * @param key 键字符串
 * @param value 值字符串
 * @return 设置成功返回0；键已存在返回正数；失败返回负数
 * @note 如果inst或key/value为NULL，则返回-1
 * @note 如果数组已满，则返回-2
 * @note 如果key内存分配失败，则返回-3
 * @note 如果value内存分配失败，则返回-4
 * @note 如果键不存在，则添加新的键值对并返回0
 * @note 如果键已存在且值相同，则返回1
 * @note 如果键已存在但值不同，则更新值并返回0
 */
int kvs_array_set(kvs_array_t *inst, char *key, char *value)
{
	// 参数检查
	if (inst == NULL || key == NULL || value == NULL)
	{
		DEBUG("Invalid arguments to kvs_array_set\n");
		return -1;
	}

	// 检查数组是否已满
	if (inst->total == KVS_ARRAY_SIZE)
	{
		DEBUG("Array is full, cannot add more items\n");
		return -2;
	}

	// 检查键是否已存在并比较替换替换值
	int i = 0;
	for (i = 0; i < inst->total; i++)
	{
		if (inst->table[i].key != NULL && strcmp(inst->table[i].key, key) == 0)
		{
			DEBUG("Key already exists: %s\n", key);
			if (strcmp(inst->table[i].value, value) == 0)
			{
				DEBUG("Value is the same, no need to update\n");
				return 1; // 键已存在且值相同
			}
			kvs_free(inst->table[i].value); // 释放旧值
			inst->table[i].value = kvs_malloc(strlen(value) + 1);
			if (inst->table[i].value == NULL)
			{
				DEBUG("Failed to allocate memory for value copy\n");
				return -4;
			}
			memset(inst->table[i].value, 0, strlen(value) + 1);
			strncpy(inst->table[i].value, value, strlen(value));
			DEBUG("Updated key: %s with new value: %s\n", key, value);
			return 0; // 成功替换
		}
	}

	// 分配内存并复制键和值
	char *kcopy = kvs_malloc(strlen(key) + 1);
	if (kcopy == NULL)
	{
		DEBUG("Failed to allocate memory for key copy\n");
		return -3;
	}
	memset(kcopy, 0, strlen(key) + 1);
	strncpy(kcopy, key, strlen(key));

	char *kvalue = kvs_malloc(strlen(value) + 1);
	if (kvalue == NULL)
	{
		DEBUG("Failed to allocate memory for value copy\n");
		kvs_free(kcopy);
		return -4;
	}
	memset(kvalue, 0, strlen(value) + 1);
	strncpy(kvalue, value, strlen(value));

	// 查找第一个空闲位置
	for (i = 0; i < inst->total; i++)
	{
		if (inst->table[i].key == NULL)
		{
			inst->table[i].key = kcopy;
			inst->table[i].value = kvalue;
			inst->total++;
			DEBUG("Added key: %s, value: %s at index %d\n", key, value, i);
			return 0; // 成功添加
		}
	}

	// 如果没有找到空闲位置且数组未满
	if (i == inst->total && i < KVS_ARRAY_SIZE)
	{
		inst->table[i].key = kcopy;
		inst->table[i].value = kvalue;
		inst->total++;
		DEBUG("Added key: %s, value: %s at index %d\n", key, value, i);
		return 0; // 成功添加
	}

	// 如果走到这里，说明数组已满或发生了其他错误
	kvs_free(kcopy);
	kvs_free(kvalue);
	DEBUG("Something went wrong, could not add key-value pair\n");
	return -5;
}

/**
 * @brief 获取KVS数组中指定键的值
 * @param inst 指向kvs_array_t实例的指针
 * @param key 键字符串
 * @return 返回找到的值，如果未找到则返回NULL
 * @note 如果inst或key为NULL，则返回NULL
 * @note 如果键不存在，则返回NULL
 * @note 如果找到键，则返回对应的值指针
 */
char *kvs_array_get(kvs_array_t *inst, char *key)
{
	if (inst == NULL || key == NULL)
	{
		DEBUG("Invalid arguments to kvs_array_get\n");
		return NULL;
	}

	int i = 0;
	for (i = 0; i < inst->total; i++)
	{
		if (inst->table[i].key != NULL && strcmp(inst->table[i].key, key) == 0)
		{
			DEBUG("Found key: %s with value: %s at index %d\n", key, inst->table[i].value, i);
			return inst->table[i].value; // 返回找到的值
		}
	}

	DEBUG("Key not found: %s\n", key);
	return NULL; // 未找到键
}

/**
 * @brief 删除KVS数组中指定键的键值对
 * @param inst 指向kvs_array_t实例的指针
 * @param key 键字符串
 * @return 成功删除返回0，未找到键返回1，失败返回负数
 * @note 如果inst或key为NULL，则返回-1
 * @note 如果键不存在，则返回1
 * @note 如果成功删除键值对，则返回0
 */
int kvs_array_del(kvs_array_t *inst, char *key)
{
	if (inst == NULL || key == NULL)
	{
		DEBUG("Invalid arguments to kvs_array_del\n");
		return -1;
	}
	int i = 0;
	for (i = 0; i < inst->total; i++)
	{
		if (inst->table[i].key != NULL && strcmp(inst->table[i].key, key) == 0)
		{
			DEBUG("Deleting key: %s at index %d\n", key, i);
			kvs_free(inst->table[i].key);
			inst->table[i].key = NULL;
			kvs_free(inst->table[i].value);
			inst->table[i].value = NULL;

			// 如果删除的是最后一个元素，减少总数
			// 不需要移动，因为添加的时候会检验前面是否有空位
			if (inst->total - 1 == i)
			{
				inst->total--;
				DEBUG("Deleted last item, total now: %d\n", inst->total);
			}
			return 0; // 成功删除
		}
	}
	DEBUG("Key not found for deletion: %s\n", key);
	return 1; // 未找到键
}

/**
 * @brief 修改KVS数组中指定键的值
 * @param inst 指向kvs_array_t实例的指针
 * @param key 键字符串
 * @param value 新的值字符串
 * @return 成功返回0，未找到键返回1，失败返回负数
 * @note 如果inst或key/value为NULL，则返回-1
 * @note 如果键不存在，则返回1
 * @note 如果成功修改键值对，则返回0
 */
int kvs_array_mod(kvs_array_t *inst, char *key, char *value)
{
	if (inst == NULL || key == NULL || value == NULL)
	{
		DEBUG("Invalid arguments to kvs_array_mod\n");
		return -1;
	}
	int i = 0;
	for (i = 0; i < inst->total; i++)
	{
		if (inst->table[i].key != NULL && strcmp(inst->table[i].key, key) == 0)
		{
			DEBUG("Modifying key: %s at index %d with new value: %s\n", key, i, value);
			kvs_free(inst->table[i].value); // 释放旧值
			inst->table[i].value = kvs_malloc(strlen(value) + 1);
			if (inst->table[i].value == NULL)
			{
				DEBUG("Failed to allocate memory for new value\n");
				return -2; // 内存分配失败
			}
			memset(inst->table[i].value, 0, strlen(value) + 1);
			strncpy(inst->table[i].value, value, strlen(value));
			DEBUG("Successfully modified key: %s with new value: %s\n", key, value);
			return 0; // 成功修改
		}
	}
	DEBUG("Key not found for modification: %s\n", key);
	return 1; // 未找到键
}

/**
 * @brief 检查KVS数组中是否存在指定键
 * @param inst 指向kvs_array_t实例的指针
 * @param key 键字符串
 * @return 如果键存在返回0，不存在返回1，失败返回-1
 * @note 如果inst或key为NULL，则返回-1
 */
int kvs_array_exist(kvs_array_t *inst, char *key)
{
	if (inst == NULL || key == NULL)
	{
		DEBUG("Invalid arguments to kvs_array_exist\n");
		return -1;
	}

	char *value = kvs_array_get(inst, key);
	if (value != NULL)
	{
		DEBUG("Key exists: %s with value: %s\n", key, value);
		return 0; // 键存在
	}

	DEBUG("Key does not exist: %s\n", key);
	return 1; // 键不存在
}
