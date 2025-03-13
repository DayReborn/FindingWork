[TOC]



# Python笔记

## 基础知识学习











## 算法介绍

### 一、复杂度

#### 1.1 复杂度介绍

* 时间复杂度是估算运行时间的一个式子

* **一般来说**，时间复杂度高的算法相较于时间复杂度低的算法慢

* 常见的时间复杂度为：
  $$
  O(1)<O(\log n)<O(n)<O(n\log n)<O(n^2)<O(n^2\log n)<O(n^3)
  $$

* 复杂问题的时间复杂度：
  $$
  \mathrm{~O(n!)~O(2^n)~O(n^n)~\ldots}
  $$
  
* 





#### 1.2 如何判断时间复杂度

* 快速判断算法复杂度：
  * 确定问题规模  -->  $n$
  * 循环减半过程  -->  $\log n$
  * k层关于n的循环  -->  $n^k$
* 复杂情况：根据算法执行过程判断





#### 1.3 空间复杂度

空间复杂度：用来评估算法内存占用大小的式子

空间复杂度和时间复杂度的表示方法完全一致

	* 算法使用了几个变量：$O(1)$
	* 算法使用了长度为n的一维列表：$O(n)$
	* 算法使用了m行n列的二位列表：$O(mn)$

==**“空间换时间”非常重要**==

---







### 二、递归

#### 2.1 递归介绍

递归的两个特点：

	* 调用自身
	* **结束条件**

**func1**和**func2**不是递归：

```python
def func1(x):
    print(x)
    func(x)

def func2(x):
if x > 0:
    func(x + 1)
    print(x)
```

**func3**和**func4**两个递归需要区分输出结果：

```python
def func3(x):  # 对于func3(3)输出为：1 2 3
    if x > 0:
        func(x - 1)
        print(x)
        
def func4(x):  # 对于func4(3)输出为：3 2 1
    if x > 0:
        print(x)
        func(x - 1)
```





#### 2.2 汉诺塔问题

**问题前导：**

- 大梵天创造世界的时候做了三根金刚石柱子，在一根柱子上从下往上按照大小顺序摞着64片黄金圆盘。
- 大梵天命令婆罗门把圆盘从下面开始按大小顺序重新摆放在另一根柱子上。
- 在小圆盘上不能放大圆盘，在三根柱子之间一次只能移动一个圆盘
- 64根柱子移动完毕之日，就是世界毁灭之时。



**递归算法设计：**

n个盘子时（从A经过C移动到B）：

1. 将n-1个圆盘从A移到C
2. 将第n个圆盘从A移到B
3. 将n-1个圆盘从C移到B

（其中1、3两步涉及到n-1的递归）



**递归代码实现：**

```python
def hanoi(n, start, middle, end):
    if n > 0:
        hanoi(n - 1, start, end, middle)
        print('moving form %s to %s' % (start, end))
        hanoi(n - 1, middle, start, end)

hanoi(10, 'A', 'C', 'B')
```



**最终次数计算：**

- 移动次数递归：$h(x) = 2h(x-1)+1$
- $h(64) = 18446744073709551615$
- 假设一秒搬一个盘子，总共需要5800亿年

---







### 三、查找

#### 3.1 查找介绍

- 查找：在一些数据元素中，通过一定的方法找出与给定关键字相同的数据元素的过程。
- 列表查找 (线性表查找): 从列表中查找指定元素 
  - **输入：列表、待查找元素**
  - **输出：元素下标 (未找到元素时一般返回None或-1)**
  - **python中==找不到会报错==**
- 内置列表查找函数：`index()` 





#### 3.2 顺序查找

**顺序查找：**也叫线性查找，从列表第一个元素开始，顺序进行搜索，直到找到元素或搜索到列表最后一个元素为止。



**代码实现：**

```python
def linear_search(ilist, val):
    for ind, v in enumerate(ilist):  # enumerate()生成一个序号和元素组合的可迭代对象
        if v == val:
            print(ind)

mylist = [1, 5, 6, 4, 4, 7, 4, 8]
linear_search(mylist, 4)
```



**时间复杂度：$O(n)$**





#### 3.3 二分查找

**二分查找：**又叫折半查找，从有序列表的初始候选区 $li[0:n]$ 开始，通过对待查找的值与候选区中间值的比较，可以使候选区减少一半。



**代码实现：**

```python
def binary_search(ilist, val):
    start = 0
    end = len(ilist) - 1
    while start <= end:  # 候选框有值
        mid = (start + end) // 2
        if ilist[mid] == val:
            return mid
        elif ilist[mid] < val:  # 查找值在中值右侧
            start = mid + 1
        else:  # 查找值在中值左侧
            end = mid - 1
    else:
        return None

mylist = [1, 2, 3, 4, 5, 6, 7, 8, 9]
print(binary_search(mylist, 4))
```



**时间复杂度：$O(\log n)$**





#### 3.4 二分查找与顺序查找的比较

观察两者的复杂度$O(\log n)<O(n)$，在足够大的列表情况下（假设为40亿长度），**二分查找仅需要至多32次即可，但线性查找则需要40亿次。**



==系统内置的`index()`函数则是顺序查找，因为二分查找需要列表为顺序==

---







### 四、排序

#### 4.1 排序介绍

- 排序：将一组“无序”的记录序列调整为“有序”的记录序列。
- 列表排序：将无序列表变为有序列表
  - **输入：列表**
  - **输出：有序列表**
- 升序与降序
- 内置排序函数：`sort() # 改变原列表` `sorted() # 不改变原列表`



**常见排序：**

| Low B排序方法 | NB排序方法 | 其他排序方法 |
| :-----------: | :--------: | :----------: |
|   冒泡排序    |  快速排序  |   希尔排序   |
|   选择排序    |   堆排序   |   基数排序   |
|   插入排序    |  归并排序  |   计数排序   |





#### 4.2 冒泡排序

**冒泡排序(Bubble Sort):**

1. 列表每两个相邻的数，如果前面比后面大，则交换这两个数。
2. 一趟排序完成后，则无序区减少一个数，有序区增加一个数。
3. **代码关键点：趟、无序区范围**



**代码实现：**

```python
import random

def bubble_sort(li):
    inlist = li[:]
    num = len(inlist)  # 初始需要排序长度极为列表长度
    while num > 1:  # 设置终止条件，只有当待排序长度为2以上时才需要排序
        for i in range(num - 1):
            if inlist[i] > inlist[i + 1]:
                inlist[i], inlist[i + 1] = inlist[i + 1], inlist[i]
        num -= 1  # 无序列表长度-1
    return inlist


mylist = [random.randint(0, 1000) for i in range(100)]
print(mylist)
print(bubble_sort(mylist))
```



**时间复杂度：$O(n^2)$**



**可优化点：**

==当一趟流程中，如果没有发生交换，则说明排序已经完成。==

```python
import random

def bubble_sort(li):
    inlist = li[:]
    num = len(inlist)  # 初始需要排序长度极为列表长度
    while num > 1:  # 设置终止条件，只有当待排序长度为2以上时才需要排序
        exchange = False  # ******交换标志位******
        for i in range(num - 1):
            if inlist[i] > inlist[i + 1]:
                inlist[i], inlist[i + 1] = inlist[i + 1], inlist[i]
                exchange = True  # ******交换标志位******
        num -= 1  # 无序列表长度-1
        if not exchange:
            return inlist
    return inlist


mylist = [random.randint(0, 1000) for i in range(100)]
print(mylist)
print(bubble_sort(mylist))
```



**当然，标准化一点来写的话（额外创建列表会浪费内存空间）：**

```python
import random

def bubble_sort(inlist):
    for i in range(len(inlist)):
        exchange = False  # ******交换标志位******
        for j in range(len(inlist) - i - 1):
            if inlist[j] > inlist[j + 1]:
                inlist[j], inlist[j + 1] = inlist[j + 1], inlist[j]
                exchange = True  # ******交换标志位******
        if not exchange:
            return inlist
    return inlist


mylist = [random.randint(0, 1000) for i in range(100)]
print(mylist)
print(bubble_sort(mylist))
```





#### 4.3 选择排序

**选择排序：**

- 一趟排序记录最小的数，放到第一个位置
- 再一趟排序记录记录列表无序区最小的数，放到第二个位置
- $\dots\dots$



**算法关键点：**有序区和无序区、无序区最小数的位置



**代码实现：**

1. **简单**

```python
def select_sort(inlist):
    list_new = []
    for i in range(len(inlist)):  # range创建的是一个不可变的对象
        min_val = min(inlist)
        list_new.append(min_val)
        inlist.remove(min_val)
    return list_new


mylist = [random.randint(0, 1000) for i in range(10)]
print(mylist)
print(select_sort(mylist))
```



**时间复杂度：$O(n^2)$**



2. **优化排序算法：**不用新建列表，同时`min()`和`remove()`这两个操作的时间复杂度都是：$O(n)$。
   由此进行一次算法更新：

```python
def select_sort(inlist):
    for i in range(len(inlist)):  
        loc = i
        for j in range(i + 1, len(inlist)):
            if inlist[loc] > inlist[j]:
                loc = j
        if loc != i:
            inlist[i], inlist[loc] = inlist[loc], inlist[i]
    return inlist


mylist = [random.randint(0, 1000) for i in range(10)]
print(mylist)
print(select_sort(mylist))
```



**时间复杂度：$O(n^2)$**，但是比简单算法有所优化。



 

#### 4.4 插入排序

**插入排序：类似摸牌**

- 初始时手里(有序区)只有一张牌
- 每次(从无序区)摸一张牌，插入到手里已有牌的正确位置



**代码实现：**

1. 这是一个自己写的版本

```python
def insert_sort(inlist):
    for i in range(1, len(inlist)):
        temp = inlist[i]
        for j in range(i):
            if inlist[i - j - 1] > temp:
                inlist[i - j] = inlist[i - j - 1]
                if j == i - 1:
                    inlist[0] = temp
            else:
                inlist[i - j] = temp
                break
    return inlist


mylist = [random.randint(0, 1000) for i in range(10)]
print(mylist)
print(insert_sort(mylist))
```



2. 相对简洁的版本(代码量，循环逻辑都有所优化)

```python
def insert_sort(inlist):
    for i in range(1, len(inlist)):
        temp = inlist[i]
        j = i - 1
        while j >= 0 and inlist[j] > temp:
            inlist[j + 1] = inlist[j]
            j -= 1
        inlist[j + 1] = temp
    return inlist


mylist = [random.randint(0, 1000) for i in range(10)]
print(mylist)
print(insert_sort(mylist))
```



**时间复杂度：$O(n^2)$**





#### 4.5 快速排序

- **快速排序特点：==快==**
- **快速排序思路：**
  1. 取一个元素p (第一个元素) ,使元素p归位；
  2. 列表被p分成两部分，左边都比p小，右边都比p大；
  3. 递归完成排序。



**代码实现：**

代码实现主要分两个部分：

1. 首先是==**定位模块**==：
   - 取第一个数，并将整个列表以这个数为基准进行大小的划分。最终返回定位的位置

```python
def quick_sort_position(inlist, start, end):
    temp = inlist[start]
    while start < end:
        while end > start and inlist[end] >= temp:  # 从右到左找比暂存小的数
            end -= 1
        inlist[start] = inlist[end]  # 将比暂存小的数从右写到左空位
        while end > start and inlist[start] <= temp:  # 从左往右找比暂存大的数
            start += 1
        inlist[end] = inlist[start]  # 将比暂存大的数从左写到右空位
    inlist[start] = temp  # 暂存数归位
    return start
```



2. 第二部分是快速排序==**整体框架**==：
   - 划分整个输入序列（以第一个数为基准）
   - 迭代左侧序列
   - 迭代右侧序列

```python
def quick_sort(inlist, start, end):
    if start < end:  # 至少有两个数
        mid = quick_sort_position(inlist, start, end)
        quick_sort(inlist, start, mid - 1)  # 迭代左侧序列
        quick_sort(inlist, mid + 1, end)  # 迭代右侧序列
    return inlist



mylist = [random.randint(0, 1000) for i in range(10)]
print(mylist)
print(quick_sort(mylist, 0, len(mylist) - 1))
```



**时间复杂度：$O(n\log n)$**

- `quick_sort_position()`这个函数的时间复杂度：$O(n)$

- 整体`quick_sort()`迭代的次数应该为$\log n$次





#### 4.6 堆排序

##### 4.6.1 树

- 树是一种数据结构	比如说：目录结构
- 树是一种可以递归定义的数据结构
- 树是由n个节点组成的集合：
  - 如果n=0, 那这是一棵空树；
  - 如果n>0, 那存在1个节点作为树的根节点，其他节点可以分为m个集合，每个集合本身又是一棵树。



- **一些概念：**
  - **根节点、叶子节点：**很好理解，初始第一个节点（根）和到头的节点（叶子）
  - **树的深度（高度）：**其实就是从根节点到叶节点的最大层数
  - **树的度：** 节点中分叉数最多的节点的分叉数（度）
  - **孩子节点、父节点：**就是传导的前后节点
  - **子树：**掰下来一个树枝也可以看成一个完整的树





##### 4.6.2 二叉树

**一、二叉树定义**

1. **度不超过2的树**
2. 每个节点最多有两个孩子节点
3. 两个孩子节点被区分为左孩子节点和右孩子节点

![二叉树](D:\文件\蓝桥杯\figure\二叉树.png)

**二、 二叉树的分类**

1. **满二叉树：**一个二叉树，如果每一个层的结点数都达到最大值，则这个二叉树就是满二叉树。
2. **完全二叉树：**叶节点只能出现在最下层和次下层，并且最下面一层的结点都集中在该层最左边的若干位置的二叉树。

 ![二叉树分类](D:\文件\蓝桥杯\figure\二叉树分类.png)



 **三、二叉树的存储方式（表示方法）**

1. 链式存储方式——==数据结构部分讲解
2. 顺序存储方式——==堆排序部分来讲（列表）==



**四、二叉树的顺序存储方式（完全二叉树）**

1. **父节点**和**左孩子节点**的编号下标有什么关系？ 

   - $0\to1,1\to3,2\to5,3\to7,4\to9$
   - ==$i\to2i+1$==

   

2. **父节点**和**右孩子节点**的编号下标有什么关系？

   - $0\to2,1\to4,2\to6,3\to8,4\to10$
   - ==$i\to2i+2$==

   

3. 从孩子节点推导父节点？

   - ==$i\to(i-1)//2$==

   



##### 4.6.3 堆和堆的向下调**整**

**一、堆的定义**

1. **堆：**一种特殊的==**完全二叉树**==结构
2. **大根堆：**一棵完全二叉树，满足任一节点都比其孩子节点大
3. **小根堆：**一棵完全二叉树，满足任一节点都比其孩子节点小



**二、堆的向下调整性质**

- 假设：节点的左右子树都是堆，但自身不是堆。
- 当根节点的左右子树都是堆的时候，可以通过一次**==向下的调整==**来将其变成一个堆。





##### 4.6.4 堆排序过程

**一、堆排序过程如下：**

1. 建立大根堆。
2. 得到堆顶元素，为最大元素
3. 去掉堆顶，**==将堆最后一个元素放到堆顶==**，此时可通过一次调整重新使堆有序。
   **（避免出现非完全二叉树的问题）**
4. 堆顶元素为第二大元素。
5. 重复步骤3，直到堆变空。



**\*感觉重点在于==构建堆==和==向下调整==怎么来写**



**二、构造堆过程：（农村包围城市）**

1. 进行一个村的结构整理（构建一个至少两元素的堆）
2. 在步骤1的基础上，进行一个县的结构整理（构建一个至少四元素的堆）
3. 在步骤2的基础上，进行一个市的结构整理（构建一个至少八元素的堆）
4. $\dots \dots$



**==农村包围城市——实际上完成了我们在向下调整时的假设，就是一个新的完全二叉树只有根结点不一定是最大的，两个左右子树都为大根堆。==**





##### 4.6.5 向下调整实现

**一、内存占用节省**

防止说内存占用过大的问题，排序出的大元素应该放置在末尾，同时标记出正确的尾部数字。



**二、代码实现**

假设，整个树为完全二叉树，左右子树都成堆，整个树不成堆。

```python
def sift(inlist, low, high):  # sift()筛子筛选——这里可以理解为把小数字一直漏到底为止
    '''
    :param inlist:输入列表
    :param low:根节点位置
    :param high:最后一个叶节点位置
    :return:
    '''
    father = low  # father一开始指向根节点
    son_select = 2 * father + 1  # 选择子节点一开始为对应左子节点
    temp = inlist[low]  # 存储堆顶（只有堆顶这个数字不满足于整个树为大根堆）
    while son_select <= high:  # 当选择左子节点存在数字时
        if son_select + 1 <= high and inlist[son_select] < inlist[son_select + 1]:
            son_select = son_select + 1  # 如果右子节点大于左子节点时选择左子节点
        if inlist[son_select] > temp:
            inlist[father] = inlist[son_select]  # 选择子节点数据升级
            father = son_select  # 向下继续判断
            son_select = 2 * father + 1
        else:  # temp可以干father位置的活
            inlist[father] = temp  # 可以干活就赋值
            break
    else:
        inlist[father] = temp  # 越界说明father指向叶节点位置，直接赋值
    return inlist
```





##### 4.6.5 堆排序实现

1. 建立大根堆。
2. 得到堆顶元素，为最大元素
3. 去掉堆顶，**==将堆最后一个元素放到堆顶==**，此时可通过一次调整重新使堆有序。
   **（避免出现非完全二叉树的问题）**
4. 堆顶元素为第二大元素。
5. 重复步骤3，直到堆变空。



**代码实现：**

- **堆的建立：**

**==农村包围城市——实际上完成了我们在向下调整时的假设，就是一个新的完全二叉树只有根结点不一定是最大的，两个左右子树在前置步骤后都为大根堆。==**

```python
def heap_crate(inlist):
    n = len(inlist)
    for i in range((n - 2) // 2, -1, -1):  # 从第一个非叶节点开始迭代
        sift(inlist, i, n - 1)  # 构建大根堆
        # print(inlist)
    return inlist
```



- **堆排序实现：**

```python
def heap_sort(inlist):
    heap_crate(inlist)  # 创建初始堆
    n = len(inlist)
    for i in range(n - 1, 0, -1):  # 如果不走i=1这一次交换最终的话大根树的根节点未被交换
        inlist[i], inlist[0] = inlist[0], inlist[i]
        sift(inlist, 0, i - 1)
    return inlist


mylist = [random.randint(0, 1000) for i in range(10)]
print(mylist)
print(heap_sort(mylist))
```





##### 4.6.6 堆排序的时间复杂度

**堆排序时间复杂度：$O(n\log n)$**

- `sift()`这个函数的时间复杂度：$O(\log n)$

- `heap_crate()`这个函数循环n/2次`sift()`，则其复杂度为$O(n\log n)$

- `heap_sort()`的排序过程循环n次`sift()`，则其复杂度为$O(n\log n)$





##### 4.6.7 堆的内置模块

内置模块heapq可以得到部分堆的操作结果

```python
import heapq
import random

mylist = [random.randint(0, 1000) for i in range(10)]
print(mylist)

heapq.heapify(mylist)  # 构建一个小根堆，也是在原list基础上进行操作
print(mylist)

for i in range(len(mylist)):
    print(heapq.heappop(mylist))  # 推出最小元素
    print(mylist)  # 这种推出是在原list基础上进行的元素删除
```





##### 4.6.8 topk问题

**问题导入：**

现在有n个数, 设计算法得到前k大的数。 (k<n)



**解决思路：**

| 方法                        | 时间复杂度   |
| :-------------------------- | ------------ |
| 排序后切片                  | $O(n\log n)$ |
| 排序LowB三人组（只运行k次） | $O(nk)$      |
| 堆排序思路                  | $O(n\log k)$ |





**堆排序流程：**

1. 取列表前k个元素建立一个小根堆。堆顶就是目前第k大的数。
2. 依次向后遍历原列表，对于列表中的元素，如果小于堆顶，则忽略该元素；如果大于堆顶，则将堆顶更换为该元素，并且对堆进行一次调整；
3. 遍历列表所有元素后，倒序弹出堆顶。



**代码实现：**

- 第一部分是`sift()`函数，将原本的大根堆处理变成小根堆，代码部分只需要微调，两个不等号方向改变就行。

```python
def sift(inlist, low, high):
    '''
    :param inlist:输入列表
    :param low:根节点位置
    :param high:最后一个叶节点位置
    :return:
    '''
    father = low  # father一开始指向根节点
    son_select = 2 * father + 1  # 选择子节点一开始为对应左子节点
    temp = inlist[low]  # 存储堆顶（只有堆顶这个数字不满足于整个树为小根堆）
    while son_select <= high:  # 当选择左子节点存在数字时
        if son_select + 1 <= high and inlist[son_select] > inlist[son_select + 1]:
            son_select = son_select + 1  # 如果右子节点小于左子节点时选择左子节点
        if inlist[son_select] < temp:
            inlist[father] = inlist[son_select]  # 选择子节点数据升级
            father = son_select  # 向下继续判断
            son_select = 2 * father + 1
        else:  # temp可以干father位置的活
            inlist[father] = temp  # 可以干活就赋值
            break
    else:
        inlist[father] = temp  # 越界说明father指向叶节点位置，直接赋值
    return inlist
```



- 第二部分是`heap_crate(inlist)`，堆生成函数不要改变直接使用之前的就好

```python
def heap_crate(inlist):
    n = len(inlist)
    for i in range((n - 2) // 2, -1, -1):
        sift(inlist, i, n - 1)
        # print(inlist)
    return inlist
```



- 第三部分是topk部分代码

```python
def topk(inlist, k):
    heap = inlist[0:k]
    # 1.建堆
    heap_crate(heap)
    # 2.遍历
    for i in range(k, len(inlist)):
        if inlist[i] > heap[0]:
            heap[0] = inlist[i]
            sift(heap, 0, k-1)
    print(heap)
    # 3.输出
    for j in range(k - 1, 0, -1):  # 如果不走i=1这一次交换最终的话大根树的根节点未被交换
        heap[j], heap[0] = heap[0], heap[j]
        sift(heap, 0, j - 1)
    return heap


mylist = [random.randint(0, 1000) for i in range(10)]
print(mylist)
print(topk(mylist, 5))
```







#### 4.7 归并排序

**一、归并的定义**

假设现在的列表分两段有序，如何将其合并为一个有序列表，这种操作称为一次**归并**。



**二、一次归并（在假设两部分有序前提下使用归并）**

```python
def merge(inlist, low, mid, high):
    left = low
    right = mid + 1
    templist = []
    while left <= mid and right <= high:  # 左右都有数的时候
        if inlist[left] < inlist[right]:
            templist.append(inlist[left])
            left += 1
        else:
            templist.append(inlist[right])
            right += 1
    while left <= mid:
        templist.append(inlist[left])
        left += 1
    while right <= high:
        templist.append(inlist[right])
        right += 1
    # 切片在别的算法中不好移植
    # 这边的+=与.extend用法相同
    # 但使用+则会生成一个新的列表
    # 下面这个可以实现同样过程：
    # if left <= mid:
    #     templist += inlist[left: mid + 1]
    # if right <= high:
    #     templist += inlist[right: high + 1]
    inlist[low:high + 1] = templist
    return inlist


mylist = [1, 3, 5, 7, 9, 11, 13, 2, 4, 6, 8]
print(mylist)
print(merge(mylist, 0, 6, 10))
```



**三、实现归并排序**

- 分解：将列表越分越小，直至分成一个元素。 
- 终止条件：一个元素是有序的。
- 合并：将两个有序列表归并，列表越来越大。

![归并排序实现流程](D:\文件\蓝桥杯\figure\归并排序实现流程.png)



**代码实现：**

```python
def merge_sort(inlist, low, high):
    if low < high:  # 至少有两个数字
        mid = (low + high) // 2
        merge_sort(inlist, low, mid)
        merge_sort(inlist, mid + 1, high)
        merge(inlist, low, mid, high)
    return inlist


mylist = [random.randint(0, 1000) for i in range(10)]
print(mylist)
print(merge_sort(mylist, 0, len(mylist) - 1))
```



**四、归并排序复杂度**

**归并排序时间复杂度：$O(n\log n)$**

- `merge()`这个函数的时间复杂度：$O(n)$，仅有一次遍历全部元素操作
- 归并排序分层其复杂度为$O(\log n)$



**归并排序空间复杂度：$O(n)$**

- 归并排序不是一个**原地排序**



**python的sort方法使用的也是归并排序。**





#### 4.8 NB排序方法比较

- 三种排序算法的时间复杂度都是$O(\log n)$
- 一般情况下，就运行时间而言：
  -  快速排序 <归并排序 < 堆排序
-  三种排序算法的缺点：
  -  快速排序：极端情况下排序效率低
  -  归并排序：需要额外的内存开销
  -  堆排序：在快的排序算法中相对较慢



![排序方法比较](D:\文件\蓝桥杯\figure\排序方法比较.png)

这边注意：

1. 冒泡排序的最好情况为改进的冒泡排序

2. 快速排序的空间复杂度来自于**递归需要用到系统栈（存上一个函数的位置）**

3. **稳定性的意义：同样一个东西的相对顺序不变。**
   比如说有几个字典：
   {'name' : 'a', 'age':20}
   {'name' : 'b', 'age':25}
   {'name' : 'a', 'age':30}

   首先是按年纪排序好的，在此基础上，我要按名字排序，如果稳定就会得到{'name' : 'a', 'age':20}
   {'name' : 'a', 'age':30}
   {'name' : 'b', 'age':25}

   **有利于多关键词排序任务**





#### 4.9 希尔排序

**算法原理：**

- 希尔排序(Shell Sort)是一种分组插入排序算法。
- 首先取一个整数$d_1=n/2 $, 将元素分为$d_{1}$个组，每组相邻量元素之间距离为$d_1$, 在各组内进行直接插入排序；
-  取第二个整数$d_2=d_1/2$, 重复上述分组排序过程，直到$d_i=1$, 即所有元素在同一组内进行直接插入排序。
-  希尔排序每趟并不使某些元素有序，而是使整体数据越来越接近有序；最后一趟排序使得所有数据有序。



**代码实现：**

```python
def insert_sort_gap(inlist, gap):
    for i in range(gap, len(inlist)):
        temp = inlist[i]
        j = i - gap
        while j >= 0 and inlist[j] > temp:
            inlist[j + gap] = inlist[j]
            j -= gap
        inlist[j + gap] = temp
    return inlist


def shell_sort(inlist):
    d = len(inlist) // 2
    while d >= 1:
        insert_sort_gap(inlist, d)
        d //= 2
    return inlist


mylist = [random.randint(0, 1000) for i in range(10)]
print(mylist)
print(shell_sort(mylist))
```





**希尔排序讨论：**

整体而言比堆排序还要慢一点，具体来说希尔排序的时间复杂度要根据选取的gap序列来定，**我们选取的二分gap序列是最简单的gap序列。**





#### 4.10 计数排序

**算法原理：**

对列表进行排序，已知列表中的数范围都在0到100之间。设计时间复杂度为$O(n)$的算法。
**本质上是哈希表计数**



**代码实现：**

```python
def count_sort(inlist, max_count=100):
    count = [0 for _ in range(max_count + 1)]
    for val in inlist:
        count[val] += 1
    inlist.clear()
    for ind, val in enumerate(count):
        for _ in range(val):
            inlist.append(ind)
    # for ind, val in enumerate(count):
    #     inlist.extend([ind for _ in range(val)])
    return inlist


mylist = [random.randint(0, 1000) for i in range(100)]
print(mylist)
print(count_sort(mylist, 1000))
```





#### 4.11 桶排序

**算法原理：**

- **在计数排序中，如果元素的范围比较大 (比如在倒1亿之间) 如何改造算法？**
- **桶排序(Bucket Sort):** 首先将元素分在不同的桶中，在对每个桶中的元素排序。



**代码实现：**

```python
def bucket_sort(inlist, n=100, max_num=10000):
    d = max_num // n
    buckets = [[] for _ in range(n)]
    for val in inlist:
        ind = min(val // d, n - 1)
        buckets[ind].append(val)
        for i in range(len(buckets[ind]) - 1, 0, -1):
            if buckets[ind][i] < buckets[ind][i - 1]:
                buckets[ind][i], buckets[ind][i - 1] = buckets[ind][i - 1], buckets[ind][i]
            else:
                break
    inlist.clear()
    for buc in buckets:
        inlist.extend(buc)
    return inlist


mylist = [random.randint(0, 10000) for i in range(1000)]
print(mylist)
print(bucket_sort(mylist, 100, 10000))
```



**时间复杂度：**

- 桶排序的表现取决于数据的分布。也就是需要对不同数据排序时采取不同的分桶策略
- 平均情况时间复杂度：$O(n+k)$
- 最坏情况时间复杂度：$O(n^2k)$
- 空间复杂度：$O(nk)$

**k:由m和n计算得到，大概表示一个桶平均多长**





#### 4.12 基数排序

**算法原理：**

- 多关键字排序：加入现在有一个员工表，要求按照薪资排序，年龄相同的员工按照年龄排序。
  - 先按照年龄进行排序，再按照薪资进行**稳定的排序**。
- 对32,13,94,52,17,54,93排序，是否可以看做多关键字排序？
  - 先排个位数再排十位数



**代码实现：**

```python
def radix_sort(inlist):  # 提供类似于桶排序的思想
    max_num = max(inlist)
    n = len(str(max_num))
    for i in range(n):
        buckets = [[] for _ in range(10)]
        for val in inlist:
            buckets[val // 10 ** i % 10].append(val)
        inlist.clear()
        for buc in buckets:
            inlist.extend(buc)
    return inlist


mylist = [random.randint(0, 1000) for i in range(100)]
print(mylist)
print(radix_sort(mylist))
```



**时间复杂度：**

- 时间复杂度：$O(kn) $
- 空间复杂度：$O(k+n)$
- k表示数字位数$log(10, max)$



**基数排序吃最大数字的大小，快速排序吃数组大小，两者分情况使用。**

----











## 数据结构

### 一、数据结构介绍

#### 1.1 数据结构定义

- 数据结构是指相互之间存在着一种或多种关系的数据元素的集合和该集合中数据元素之间的关系组成。

- 简单来说，数据结构就是设计数据以何种方式组织并存储在计算机中。
  - 比如：列表、集合与字典等都是一种数据结构。
-  N.Wirth:“程序=数据结构+算法”





#### 1.2 数据结构分类

- 数据结构按照其逻辑结构可分为线性结构、树结构、图结构 
  - 线性结构：数据结构中的元素存在一对一的相互关系
  - 树结构：数据结构中的元素存在一对多的相互关系
  - 图结构：数据结构中的元素存在多对多的相互关系
    （没有完全的父到子的结构）

---







### 二、列表（顺序表）

- 列表 (其他语言称数组) 是一种基本数据类型。
- 关于列表的问题：
  - 列表中的元素是如何存储的？
  - 列表的基本操作：按下标查找、插入元素、删除元素......
  - 这些操作的时间复杂度是多少？
- 扩展：Python的列表是如何实现的？
  实际上Python的列表存储的是对应**数据的地址。**



**C语言中数组与python中列表的不同：**

1. 数组元素类型要相同
2. 数组长度固定



**常见列表操作的时间复杂度是多少？**

`append()`的时间复杂度：$O(1)$

`insert()`的时间复杂度：$O(n)$

`remove()`的时间复杂度：$O(n)$

---







### 三、栈

#### 3.1 栈的介绍

![栈示意图](D:\文件\蓝桥杯\figure\栈示意图.png)

- 栈(Stack)是一个数据集合，可以理解为只能在一端进行插入或删除操作的列表。

- 栈的特点：后进先出 LIFO (last-in, first-out) 
- 栈的概念：栈顶、栈底
- 栈的基本操作：
  - 进栈 (压栈): push 
  - 出栈：pop
  - 取栈顶：gettop



#### 3.2 栈的实现

- 使用一般的列表结构及可以实现栈：
  - 进栈：`list.append()`
  - 出栈：`list.pop()`
  - 取栈顶：`list[-1]`



**代码实现：**

```python
class Stack:
    def __init__(self):
        self.stack = []

    def push(self, element):
        self.stack.append(element)

    def pop(self):
        return self.stack.pop()

    def gettop(self):
        if len(self.stack) > 0:
            return self.stack[-1]
        else:
            return None
        
    def is_empty(self):
        return len(self.stack) == 0


stack = Stack()
stack.push(1)
stack.push(2)
stack.push(3)
print(stack.pop())
print(stack.gettop())
```





#### 3.3 栈的应用——括号匹配问题

**一、问题引入**

- **括号匹配问题：**给一个字符串，其中包含小括号、中括号、大括号、求该字符串中的括号是否匹配。
- 例如：
  - $()()[]\{\}$	 匹配
  - $([\{()\}])$	 匹配
  - $[]($		  不匹配
  - $[(])$		不匹配



**二、代码实现**

**自己的版本：**

```python
def brace_match(instr):
    stack = Stack()
    for char in instr:
        if (stack.gettop() == '{' and char == '}') or \
                (stack.gettop() == '(' and char == ')') or \
                (stack.gettop() == '[' and char == ']'):
            stack.pop()
        else:
            stack.push(char)
    return stack.is_empty()


print(brace_match('[]{}{'))
```



**课程代码版本：**

主要优势有：

- 使用字典，方便增加匹配组数
- 中间出现部分异常请跨直接中断，不需要运行到结束
- 整体代码简洁，可读性强

```python
def brace_match_1(instr):
    match = {')': '(', ']': '[', '}': '{'}
    stack = Stack()
    for char in instr:
        if char in {'(', '[', '{'}:
            stack.push(char)
        else:
            if stack.gettop() == match[char]:
                stack.pop()
            else:
                return False
    return stack.is_empty()


print(brace_match('[]{}{'))
```

---









### 四、队列

#### 4.1 队列的介绍

- 队列(Queue)是一个数据集合，仅允许在列表的一端进行插入，另一端进行删除。

- 进行插入的一端称为队尾(rear), 插入动作称为**进队或入队**
- 进行删除的一端称为队头(front), 删除动作称为**出队**
- 队列的性质：**先进先出(First-in, First-out)**

![队列](D:\文件\蓝桥杯\figure\队列.png)





#### 4.2 队列的实现

**队列能否用列表简单实现？为什么？**

![队列的实现](D:\文件\蓝桥杯\figure\队列的实现.png)

如果简单的将front+1和rear+1的话会造成空间的及大浪费，所以这里需要==**构建一个环形队列！！！！**==

![环形队列](D:\文件\蓝桥杯\figure\环形队列.png)

- 环形队列：当队尾指针front = Maxsize + 1时，再前进一个位置就自动到0
  - 队首指针前进1：$front = ( front+ 1) \% MaxSize$
  - 队尾指针前进1：$rear = ( rear+ 1) \% MaxSize$
  - 队空条件：$rear = front$
  - 队满条件：$(rear+1)\% MaxSize == front$



**代码实现：**

```python
class Queue:
    def __init__(self, maxsize=100):
        self.queue = [0 for _ in range(maxsize)]
        self.rear = 0
        self.front = 0
        self.maxsize = maxsize

    def push(self, element):
        if not self.is_filled():
            self.rear = (self.rear + 1) % self.maxsize
            self.queue[self.rear] = element
        else:
            raise IndexError('Queue is filled!')

    def pop(self):
        if not self.is_empty():
            self.front = (self.front + 1) % self.maxsize
            return self.queue[self.front]
        else:
            raise IndexError('Queue is empty!')

    def is_empty(self):
        return self.rear == self.front

    def is_filled(self):
        return (self.rear + 1) % self.maxsize == self.front


q = Queue(5)
for i in range(4):
    q.push(i)
for i in range(4):
    print(q.pop())
```

B 



#### 4.3 队列的内置模块

- 使用方法：`from collections import deque`
  - 创建队列：`queue = deque()`
  - 队尾进队：`append()`
  - 队首出队：`popleft()`
  - 双向队列队首进队：`appendleft()`
  - 双向队列队尾出队：`pop()`



#### 4.4 双向队列

- 双向队列的两端都支持进队和出队操作
- 双向队列的基本操作：
  - ADAaSDDV w a队首进队
  - 队首出队
  - 队尾进队
  - 队尾出队



**代码实现：**

```python
from collections import deque

q = deque([1, 2, 3, 4, 5, 6], maxlen=6)  # 默认生成双向列表
q.append(7)  # 队尾进队 [2, 3, 4, 5, 6, 7]  默认自动出队首
print(q.popleft())  # 队首出队 [3, 4, 5, 6, 7]
print(q.popleft())  # [4, 5, 6, 7]

# 双向列表专用
q.appendleft(3)  # 队首进队 [3, 4, 5, 6, 7]
print(q.pop())  # 队尾出队 [3, 4, 5, 6]


def tail(n):
    with open('test.txt', 'r') as f:
        q1 = deque(f, n)
        return q1


print(tail(5))

for line in tail(5):
    print(line, end='')
```

---







### 五、栈和队列应用——迷宫问题

#### 5.1 迷宫问题

给一个二维列表，表示迷宫 (0表示通道，1表示围墙) 。给出算法，求—条走出迷宫的路径。





#### 5.2 栈——深度优先搜索

**回溯法：** 

- 思路：从一个节点开始，任意找下一个能走的点，当找不到能走的点时，退回上一个点寻找是否有其他方向的点。
- **使用栈存储当前路径**



**代码实现：**

```python
import time

maze_try = [
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
    [1, 0, 0, 1, 0, 0, 0, 1, 0, 1],
    [1, 0, 0, 1, 0, 0, 0, 1, 0, 1],
    [1, 0, 0, 1, 0, 1, 1, 0, 0, 1],
    [1, 0, 1, 1, 1, 0, 0, 0, 0, 1],
    [1, 0, 0, 0, 1, 0, 0, 0, 0, 1],
    [1, 0, 1, 0, 0, 0, 1, 0, 0, 1],
    [1, 0, 1, 1, 1, 0, 1, 1, 0, 1],
    [1, 1, 0, 0, 0, 0, 0, 0, 0, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
]

dirs = [
    lambda x, y: (x - 1, y),  # 向上
    lambda x, y: (x, y + 1),  # 向右
    lambda x, y: (x + 1, y),  # 向下
    lambda x, y: (x, y - 1)  # 向左
]


def maze_path(in_maze, x1, y1, x2, y2):
    # maze = in_maze[:][:]
    maze = copy.deepcopy(in_maze)
    stack = [(x1, y1)]
    maze[x1][y1] = 2  # 防止后续退回时出现四个方向检索，最终检索到起始点的问题
    while len(stack) > 0:
        cur_node = stack[-1]  # 当前节点
        if cur_node == (x2, y2):
            return stack
        # 按顺序向四个方向进行搜索
        for dir_try in dirs:
            next_node = dir_try(cur_node[0], cur_node[1])
            if maze[next_node[0]][next_node[1]] == 0:
                stack.append(next_node)
                maze[next_node[0]][next_node[1]] = 2  # 表示该路径已走过
                break
        else:
            maze[stack[-1][0]][stack[-1][1]] = 3
            stack.pop()
        # for val in maze:
        #     print(val)
        # time.sleep(0.5)

    else:
        return False


print(maze_path(maze_try, 1, 1, 8, 8))
```



**注意问题：**

找到的路径不是最短路径！！！有的时候会绕来绕去！！！





#### 5.3 队列——广度优先搜索

**算法原理：**

- 思路：从一个节点开始，寻找所有接下来能继续走的点，继续不断寻找，

- **使用队列存储当前正在考虑的节点。**



**代码实现：**

```python
from collections import deque
import time
import copy

maze_try = [
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
    [1, 0, 0, 1, 0, 0, 0, 1, 0, 1],
    [1, 0, 0, 1, 0, 0, 0, 1, 0, 1],
    [1, 0, 0, 0, 0, 1, 1, 0, 0, 1],
    [1, 0, 1, 1, 1, 0, 0, 0, 0, 1],
    [1, 0, 0, 0, 1, 0, 0, 0, 0, 1],
    [1, 0, 1, 0, 0, 0, 1, 0, 0, 1],
    [1, 0, 1, 1, 1, 0, 1, 1, 0, 1],
    [1, 1, 0, 0, 0, 0, 0, 0, 0, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
]

dirs = [
    lambda x, y: (x - 1, y),  # 向上
    lambda x, y: (x, y + 1),  # 向右
    lambda x, y: (x + 1, y),  # 向下
    lambda x, y: (x, y - 1)  # 向左
]


def maze_path_queue(in_maze, x1, y1, x2, y2):
    maze = copy.deepcopy(in_maze)
    queue = deque()
    queue.append((x1, y1, -1))  # 添加初始位置默认为0位，第三位代表轨迹记录
    path_save = []  # 轨迹记录数组
    path_final = []
    while len(queue) > 0:
        cur_node = queue.popleft()  # 推出当前结点
        path_save.append(cur_node)  # 向轨迹记录数组存入出队元素
        if (cur_node[0], cur_node[1]) == (x2, y2):  # 如果出队元素为终点坐标
            path_final.append((x2, y2))
            ind = cur_node[2]  # 下个坐标在path_save中的索引
            while ind != -1:
                path_final.append((path_save[ind][0], path_save[ind][1]))  # 添加下个坐标
                ind = path_save[ind][2]  # 寻找下个坐标索引
            path_final.reverse()  # 输出正序坐标
            return path_final
        else:
            for dir_try in dirs:  # 四个方向生成测试
                next_node = dir_try(cur_node[0], cur_node[1])  # 生成的测试坐标
                if maze[next_node[0]][next_node[1]] == 0:  # 空位置
                    queue.append((next_node[0], next_node[1], len(path_save) - 1))  # 推入新的队列元素
                    maze[next_node[0]][next_node[1]] = 2  # 标记走过的结点
    else:
        return False


print(maze_path_queue(maze_try, 1, 1, 8, 8))
```

---







### 六、链表

#### 6.1 链表的介绍

**链表**是由一系列节点组成的元素集合。每个节点包含两部分，数据域item和指向下一个 节点的指针next。通过节点之间的相互连接，最终串联成一个链表。

![链表](D:\文件\蓝桥杯\figure\链表.png)



**代码简单实现：**

```python
class Node:
    def __init__(self, item):
        self.item = item
        self.next = None


a = Node(1)
b = Node(2)
c = Node(3)
a.next = b
b.next = c

print(a.next.next.item)
```





#### 6.2 链表的创建和遍历

**一、头插法**

```python
class Node:
    def __init__(self, item):
        self.item = item
        self.next = None


def create_linklist_head(inlist):
    head = Node(inlist[0])
    for element in inlist[1:]:
        node = Node(element)
        node.next = head
        head = node
    return head


def print_linklist(lk):
    while lk:
        print(lk.item, end=',')
        lk = lk.next


linklist = create_linklist([1, 2, 3])
print_linklist(linklist)
```

**头插法只需要维护头部！！！**



**二、尾插法**

```python
def create_linklist_tail(inlist):
    head = Node(inlist[0])
    tail = head
    for element in inlist[1:]:
        node = Node(element)
        tail.next = node
        tail = node
    return head
```

尾插法需要维护头部和尾部两个部分，不过最终输出还是输出头部。



#### 6.3 链表节点的插入和删除

**一、链表节点的插入**

 ==**分成两步走，顺序不能错！！！**==

1. `p.next = CurNode.next`
2. `curNode.next=p`

![链表插入](D:\文件\蓝桥杯\figure\链表插入.png)



**二、链表节点的删除**

==**分成三步走，顺序不能错！！！**==

1. `p = curNode.next` 
2. `curNode.next = p.next`
3. `del p`

![链表节点删除](D:\文件\蓝桥杯\figure\链表节点的删除.png)



#### 6.4 双链表

**一、双链表的定义**

- **双链表**的每个节点有两个指针：一个指向后一个节点，另一个指向前一个节点。
- 如何建立双链表？

![双链表](D:\文件\蓝桥杯\figure\双链表.png)



**二、双链表的定义**

```python
class Node:
    def __init__(self, item):
        self.item = item
        self.next = None
        self.prior = None
```





**三、双链表的插入**

1. `p.next = CurNode.next`
2. `curNode.next.prior = p`
3. `p.prior = curNode`
4. `curNode.next = p`



**四、双链表节点的删除**

1. `p = CurNode.next`
2. `curNode.next = p.next`
3. `p.next.prior = curNode`
4. `del p`





#### 6.5 链表总结

**一、复杂度分析**

- 顺序表 (列表/数组) 与链表
  - 按元素值查找
  - 按下标查找
  - 在某元素后插入
  - 删除某元素



**二、链表与顺序表**

- 链表在插入和删除的操作上明显快于顺序表
- 链表的内存可以更灵活的分配
  - **试利用链表重新实现栈和队列**
- 链表这种链式存储的数据结构对树和图的结构有很大的启发性

---







### 七、哈希表

#### 7.1 哈希表的介绍

**一、哈希表**

- 哈希表一个通过哈希函数来计算数据存储位置的数据结构，通常支持如下操作：
  - `insert(key, value)`: 插入键值对(key,value)
  - `get(key)`: 如果存在键为key的键值对则返回其value，否则返回空
  - `delete(key)`: 删除键为key的键值对



**二、直接寻址表**

当关键字的全域U比较小的时候，直接寻址是一种简单而高效的方法。

![直接寻址表](D:\文件\蓝桥杯\figure\直接寻址表.png)

直接寻址的技术缺点：

- 当域U很大时，需要消耗大量内存，很不实际
- 如果域U很大而实际出现的key很少，则大量空间被浪费

- 无法处理关键字不是数字的情况




**三、哈希**

- 直接寻址表：key为k的元素放到k位置上

- 改进直接寻址表：哈希 (Hashing)
  - 构建大小为m的寻址表T
  - key为k的元素放到h(k)位置上
  - h(k)是一个函数，其将域U映射到表T[0,1,...,m-1]



**四、哈希表**

-  哈希表 (Hash Table, 又称为散列表) , 是一种线性表的存储结构。哈希表由一个直接寻址表和一个哈希函数组成。哈希函数h(k)将元素关键字k作为自变量，返回元素的存储下标。
- 假设有一个长度为7的哈希表，哈希函数h(k)=k%7。元素集合{14,22,3,5}的存储方式如下图。

![哈希表示例](D:\文件\蓝桥杯\figure\哈希表示例.png)



**五、哈希冲突**

- 由于哈希表的大小是有限的，而要存储的值的总数量是无限的，因此对于任何哈希函数，都会出现两个不同元素映射到同一个位置上的情况，这种情况叫做**哈希冲突**。

- 比如h(k)=k\%7, h(0)=h(7)=h(14)=...



**六、解决哈希冲突**

- **开放寻址法：**如果哈希函数返回的位置已经有值，则可以向后探查新的位置来
  - **线性探查：**如果位置i被占用，则探查i+1,  i+2,......
  - **二次探查：**如果位置i被占用，则探查i+1^2^,  i-1^2^,  i+2^2^,  i-2^2^,......
  - **二度哈希：**有n个哈希函数，当使用第1个哈希函数h1发生冲突时，则尝试使用h2，h3,......



- **拉链法：**哈希表每个位置都连接一个链表，当冲突发生时，冲突的元素将被加到该位置链表的最后。**有利于删除添加**

![拉链法](D:\文件\蓝桥杯\figure\拉链法.png)



**七、常见哈希函数**

- 除法哈希法：
  - $h(k)=k\%m$
- 乘法哈希法：
  - $h(k) = floor(m*(A*key\%1))$
- 全域哈希法：
  - $h_{a,b}(k)=((a*key+b) mod\hspace{2mm} p) mod\hspace{2mm}m\hspace{10mm}a,b=1,2,...,p-1$
  - mod = %





#### 7.2 哈希表的实现

**一、实现一个完整的链表类**

```python
class LinkList:
    class Node:
        def __init__(self, item):
            self.item = item
            self.next = None

    class LinkListIterator:  # 迭代器类
        def __init__(self, node):
            self.node = node

        def __next__(self):
            if self.node:
                cur_node = self.node
                self.node = cur_node.next
                return cur_node.item
            else:
                raise StopIteration

        def __iter__(self):
            return self

    def __init__(self, iterable=None):
        self.head = None
        self.tail = None
        if iterable:
            self.extend(iterable)

    def append(self, obj):  # 添加单个节点
        append_node = LinkList.Node(obj)
        if not self.head:
            self.head = append_node
            self.tail = append_node
        else:
            self.tail.next = append_node
            self.tail = append_node

    def extend(self, iterable):  # 添加一个列表
        for obj in iterable:
            self.append(obj)

    def find(self, obj):  # 寻找节点
        for n in self:
            if n.item == obj:
                return True
        else:
            return False

    def __iter__(self):  # 迭代器
        return self.LinkListIterator(self.head)

    def __repr__(self):  # 给出输出时的返回值
        return '<<' + ','.join(map(str, self)) + '>>'


lk = LinkList([1, 2, 3, 4, 5])
print(lk)
for element in lk:
    print(element)
```



**二、实现哈希表类**

```python
class HashTable:
    def __init__(self, size=101):
        self.size = size
        self.table = [LinkList() for _ in range(self.size)]

    def my_hash(self, key):
        return key % self.size

    def insert(self, key):
        i = self.my_hash(key)
        if self.find(key):
            print('Duplicated Insert')
        else:
            self.table[i].append(key)

    def find(self, key):
        i = self.my_hash(key)
        return self.table[i].find(key)


ht = HashTable()
ht.insert(1)
ht.insert(2)
ht.insert(202)
print(ht.find(202))
```





#### 7.3 哈希表的应用

**一、字典与集合**

- **字典与集合都是通过哈希表来实现的。**
  - a= \{ 'name':Alex',  'age':18,  'gender':'Man'}
- 使用哈希表存储字典，通过哈希函数将字典的键映射为下标。假设h('name')= 3, h('age')=1, h('gender')=4, 则哈希表存储为[None, 18, None, 'Alex','Man']
- 如果发生哈希冲突，则通过拉链法或开发寻址法解决



**二、md5算法**

- MD5(Message-Digest Algorithm 5)**曾经**是密码学中常用的哈希函数，可以把任意长度的数据映射为128 位的哈希值，其曾经包含如下特征：

1. 同样的消息，其MD5值必定相同；
2. 可以快速计算出任意给定消息的MD5值；
3. 除非暴力的枚举所有可能的消息，否则不可能从哈希值反推出消息本身；
4. 两条消息之间即使只有微小的差别，其对应的MD5 值也应该是完全不同、完全不相关的；
5. 不能在有意义的时间内人工的构造两个不同的消息 使其具有相同的MD5值。



**应用举例：文件的哈希值**

算出文件的哈希值，若两个文件的哈希值相同，则可认为这两个文件是相同的。因此：

1. 用户可以利用它来验证下载的文件是否完整。
2. 云存储服务商可以利用它来判断用户要上传的文件 是否已经存在于服务器上，从而实现秒传的功能，同时避免存储过多相同的文件副本。

---







### 八、树

#### 8.1 树的介绍

- 树是一种数据结构               比如：目录结构
- 树是一种可以递归定义的数据结构
- 树是由n个节点组成的集合：
  - 如果n=0, 那这是一棵空树；
  - 如果n>0, 那存在1个节点作为树的根节点，其他节点可以分为m个集合，每个集合本身又是一棵树。

![树](D:\文件\蓝桥杯\figure\树.png)

**一些概念：**

- **根节点、叶子节点：**很好理解，初始第一个节点（根）和到头的节点（叶子）
- **树的深度（高度）：**其实就是从根节点到叶节点的最大层数
- **树的度：** 节点中分叉数最多的节点的分叉数（度）
- **孩子节点、父节点：**就是传导的前后节点
- **子树：**掰下来一个树枝也可以看成一个完整的树







#### 8.2 树的实例——模拟文件系统

**一、首先实现一个简单的节点类**

```python
class Node:
    def __init__(self, name, type='dir'):
        self.name = name
        self.type = type  # 'dir' or 'file'
        self.children = []
        self.parent = None
        # 跟链表一样链式存储
        

n1 = Node('hello')
n2 = Node('world')
n1.children.append(n2)
n2.parent = n1
```



**二、构建文件树**

```python
class FileSystemTree:
    def __init__(self):
        self.root = Node('/')
        self.now = self.root

    def mkdir(self, name):
        if name[-1] != '/':
            name += '/'
        node = Node(name)
        self.now.children.append(node)
        node.parent = self.now

    def ls(self):
        return self.now.children

    def cd(self, name):
        if name[-1] != '/':
            name += '/'
        if name == '../':
            self.now = self.now.parent
            return

        for child in self.now.children:
            if child.name == name:
                self.now = child
                return
        raise ValueError('invalid dir')


tree = FileSystemTree()
tree.mkdir('var/')
tree.mkdir('bin/')
tree.mkdir('usr/')
print(tree.ls())

tree.cd('bin/')
tree.mkdir('python')
print(tree.ls())

tree.cd('..')
print(tree.ls())
```

---







### 九、二叉树

#### 9.1 二叉树的介绍

- **二叉树的链式存储：**将二叉树的节点定义为一个对象，节点之间通过类似链表的链接方式来连接。
- **节点定义：**

```python
class BiTreeNode:
    def __init__(self, data):
        self.data = data
        self.lchild = None  # 左子节点
        self.rchild = None  # 右子节点
```

![二叉树数据结构](D:\文件\蓝桥杯\figure\二叉树数据结构.png)





#### 9.2 二叉树的遍历

- 二叉树的遍历方式：
  - 前序遍历：EACBDGF
  - 中序遍历：ABCDEGF
  - 后序遍历：BDCAFGE
  - 层次遍历：EAGCFBD



**代码实现几种遍历方式：**

```python
from collections import deque


class BiTreeNode:
    def __init__(self, data):
        self.data = data
        self.lchild = None  # 左子节点
        self.rchild = None  # 右子节点


a = BiTreeNode('A')
b = BiTreeNode('B')
c = BiTreeNode('C')
d = BiTreeNode('D')
e = BiTreeNode('E')
f = BiTreeNode('F')
g = BiTreeNode('G')

e.lchild = a
a.rchild = c
c.lchild = b
c.rchild = d
e.rchild = g
g.rchild = f

root = e


def pre_order(root):
    if root:
        print(root.data, end='')
        pre_order(root.lchild)
        pre_order(root.rchild)


def in_order(root):
    if root:
        in_order(root.lchild)
        print(root.data, end='')
        in_order(root.rchild)


def post_order(root):
    if root:
        post_order(root.lchild)
        post_order(root.rchild)
        print(root.data, end='')


def level_order(root):
    if root:
        queue = deque([root])
        while queue:
            node = queue.popleft()
            print(node.data, end='')
            if node.lchild:
                queue.append(node.lchild)
            if node.rchild:
                queue.append(node.rchild)


pre_order(root)
print('')
in_order(root)
print('')
post_order(root)
print('')
level_order(root)
```





#### 9.3 二叉搜索树的概念

**一、二叉搜索树概念**

- **二叉搜索树**是一颗二叉树且满足性质：
  - 设x是二叉树的一个节点。
  - 如果y是x左子树的一个节点，那么y.key <x.key;
  - 如果y是x右子树的一个节点，那么y.key > x.key。
- 二叉搜索树的操作：**查询、插入、删除**

![二叉搜索树](D:\文件\蓝桥杯\figure\二叉搜索树.png)



**请注意:         8、9都应该小于等于11。      8、9、11都应该大于等于5**



#### 9.4 二叉搜索树的遍历

```python
class BiTreeNode:
    def __init__(self, data):
        self.data = data
        self.lchild = None  # 左子节点
        self.rchild = None  # 右子节点
        self.parent = None  # 父节点


class BST:
    def __init__(self, inlist=None):
        self.root = None
        for val in inlist:
            self.insert_no_rec(val)
            # if not self.root:
            #     self.root = BiTreeNode(val)
            # else:
            #     self.insert(self.root, val)

            
    def insert(self, node, val):  # 递归实现输入
        if not node:
            node = BiTreeNode(val)
        elif val < node.data:
            node.lchild = self.insert(node.lchild, val)
            node.lchild.parent = node
        else:
            node.rchild = self.insert(node.rchild, val)
            node.rchild.parent = node
        return node

    def insert_no_rec(self, val):
        node = self.root
        if not node:
            self.root = BiTreeNode(val)
            return
        while True:
            if val < node.data:
                if not node.lchild:
                    node.lchild = BiTreeNode(val)
                    node.lchild.parent = node
                    return
                else:
                    node = node.lchild
            else:
                if not node.rchild:
                    node.rchild = BiTreeNode(val)
                    node.rchild.parent = node
                    return
                else:
                    node = node.rchild

    def pre_order(self, root):
        if root:
            print(root.data, end='')
            self.pre_order(root.lchild)
            self.pre_order(root.rchild)

    def in_order(self, root):
        if root:
            self.in_order(root.lchild)
            print(root.data, end='')
            self.in_order(root.rchild)

    def post_order(self, root):
        if root:
            self.post_order(root.lchild)
            self.post_order(root.rchild)
            print(root.data, end='')


tree = BST([4, 3, 2, 5, 8, 9, 1, 6, 7])
tree.pre_order(tree.root)  # 432158679
print('')
tree.in_order(tree.root)  # 123456789
print('')
tree.post_order(tree.root)  # 123769854
```



- 结合中序遍历的定义和二叉搜索树的定义可知，**中序遍历一定是从小到大输出结果。**



#### 9.5 二叉搜索树的查询

```python
    def query(self, node, val):
        if not node:
            return None
        elif node.data < val:
            return self.query(node.rchild, val)
        elif node.data > val:
            return self.query(node.lchild, val)
        else:
            return node

    def query_no_rec(self, val):
        node = self.root
        while node:
            if node.data < val:
                node = node.rchild
            elif node.data > val:
                node = node.lchild
            else:
                return node
        else:
            return None


print(tree.query_no_rec(4))
# <__main__.BiTreeNode object at 0x0000018AE75E1250>
print(tree.query(tree.root, 11))
# None
```





#### 9.6 二叉搜索树的删除

**删除操作分为三类：**

1. 如果要删除的节点是叶子节点：直接删除
   ![BST删除1](D:\文件\蓝桥杯\figure\BST删除1.png)
2. 如果要删除的节点只有一个孩子：将此节点的父亲与孩子连接，然后删除该节点。
   ![BST删除2](D:\文件\蓝桥杯\figure\BST删除2.png)
3. 如果要删除的节点有两个孩子：将其右子树的最小节点 （该节点最多有一个右孩子）删除，并替换当前节点。
   ![image-20240229213411808](D:\文件\蓝桥杯\figure\BST删除3.png)



**代码实现：**

```python
    def __remove_node_1(self, node):
        # 情况1：node是叶子节点
        if not node.parent:
            self.root = None
        if node == node.parent.lchild:
            node.parent.lchild = None
        else:
            node.parent.rchild = None

    def __remove_node_21(self, node):
        # 情况2.1：node只有一个左子节点
        if not node.parent:
            self.root = node.lchild
        if node == node.parent.lchild:
            node.parent.lchild = node.lchild
            node.lchild.parent = node.parent
        else:
            node.parent.rchild = node.lchild
            node.lchild.parent = node.parent

    def __remove_node_22(self, node):
        # 情况2.2：node只有一个右子节点
        if not node.parent:
            self.root = node.rchild
        if node == node.parent.lchild:
            node.parent.lchild = node.rchild
            node.rchild.parent = node.parent
        else:
            node.parent.rchild = node.rchild
            node.rchild.parent = node.parent

    def delete(self, val):
        if self.root:  # 不是空树
            node = self.query_no_rec(val)
            if not node:  # 不存在
                return False
            if not node.lchild and not node.rchild:  # 1.叶子节点
                self.__remove_node_1(node)
            elif not node.rchild:  # 2.只有左子节点
                self.__remove_node_21(node)
            elif not node.lchild:  # 3.只有右子节点
                self.__remove_node_22(node)
            else:  # 4.左右子节点都有
                min_node = node.rchild
                while min_node.lchild:
                    min_node = min_node.lchild
                node.data = min_node.data
                # 删除min_node
                if not min_node.rchild:
                    self.__remove_node_1(min_node)
                else:
                    self.__remove_node_22(min_node)


tree = BST([4, 3, 2, 5, 8, 9, 1, 6, 7])
tree.in_order(tree.root)
print('')

print(tree.delete(4))
tree.in_order(tree.root)
```





### 十、AVL







## 算法进阶

### 一、贪心算法

#### 1.1 算法介绍

- **贪心算法 (又称贪婪算法)**是指，在对问题求解时，总是做出在当前看来是最好的选择。也就是说，不从整体最优上加以考虑，他所做出的是在某种意义上的局部最优解。
- 贪心算法并不保证会得到最优解，但是在**某些问题上贪心算法的解就是最优解**。要会判断一个问题能否用贪心算法来计算。





#### 1.2  找零问题

**问题引入**

- 假设商店老板需要找零n元钱，钱币的面额有：100元、50元、20元、5元、1元，如何找零使得所需钱币的数量最少？



**代码实现**

```python
t = [100, 50, 20, 5, 1]


def change(t, n):
    num = [0 for _ in range(len(t))]
    for i, money in enumerate(t):
        num[i] = n // money
        n = n % money
    return num, n


print(change(t, 375))
```

---







### 二、背包问题

#### **1.1 问题引入**

- 一个小偷在某个商店发现有n个商品，第i个商品价值vi元，重wi千克。他希望拿走的价值尽量高，但他的背包最多只能容纳W千克的东西。他应该拿走哪些商品？
  - **0-1背包：**对于一个商品，小偷要么把它完整拿走，要么留下。不能只拿走一部分，或把一个商品拿走多次。 (商品为金条)
  - **分数背包：**对于一个商品，小偷可以拿走其中任意一部分。 (商品为金砂)





- 举例：
  - 商品1：V~1~ = 60  W~1~ = 10
  - 商品2：V~2~ = 100  W~2~ = 20
  - 商品3：V~3~ = 120  W~3~ = 30
  - 背包容量：W = 50



**思考！！！对于0-1背包和分数背包，贪心算法是否都能得到最优解？为什么？**





#### 1.2 分数背包

**分数背包可以用贪心算法得到最优解。**



**代码实现：**

```python
goods_test = [(100, 20), (60, 10), (120, 30)]


def fractional_backpack(goods, w):
    goods_new = [[index, item] for index, item in enumerate(goods)]  # 保留原始序号！！！
    goods_new.sort(key=lambda x: x[1][0] / x[1][1], reverse=True)
    m = [0 for _ in range(len(goods))]
    total_value = 0
    for [index_orgin, (value, weight)] in goods_new:
        if weight < w:
            m[index_orgin] += weight
            w -= weight
            total_value += value
        else:
            m[index_orgin] += w
            total_value += value * w / weight
            break

    return m, round(total_value)


print(fractional_backpack(goods_test, 50))
```





### 三、拼接最大数字问题

- 一有n个非负整数，将其按照字符串拼接的方式拼接为一个整数。
  如何拼接可以使得得到的整数最大？ 
- 例：32,94,128,1286,6,71可以拼接除的最大整数为94716321286128



**代码实现：**

```python
from functools import cmp_to_key

list_test = [32, 94, 128, 1286, 6, 71]


def xy_cmp(x, y):
    if x + y < y + x:
        return 1
    elif x + y > y + x:
        return -1
    else:
        return 0


def number_join(inlist):
    inlist = list(map(str, inlist))
    inlist.sort(key=cmp_to_key(xy_cmp))
    return ''.join(inlist)


print(number_join(list_test))
```





### 四、活动选择问题

- 假设有n个活动，这些活动要占用同一片场地，而场地在某时刻只能供一个活动使用。

- 每个活动都有一个开始时间s~i~和结束时间f~i~ (题目中时间以整数表示),表示活动在[s~i~, f~i~)区间占用场地。

- 问：安排哪些活动能够使该场地举办的活动的个数最多？

|    i     |  1   |  2   |  3   |  4   |  5   |  6   |  7   |  8   |  9   |  10  |  11  |
| :------: | :--: | :--: | :--: | :--: | :--: | :--: | :--: | :--: | :--: | :--: | :--: |
| **s~i~** |  1   |  3   |  0   |  5   |  3   |  5   |  6   |  8   |  8   |  2   |  12  |
| **f~i~** |  4   |  5   |  6   |  7   |  9   |  9   |  10  |  11  |  12  |  14  |  16  |

