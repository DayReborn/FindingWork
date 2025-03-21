#include <iostream>
#include <cassert>

// 步骤 2.1：定义模板类
template <typename T>
class Vector {
private:
    T* data_ = nullptr;       // 数据指针
    size_t size_ = 0;         // 当前元素数量
    size_t capacity_ = 0;     // 当前分配的内存容量

public:
    // 默认构造函数
    Vector() = default;

    // 析构函数
    ~Vector() {
        delete[] data_;
    }


    //==========================================================
    // 步骤 2.2：实现push_back和扩容
    void push_back(const T& value) {
        if (size_ >= capacity_) {
            // 扩容策略：容量为0时设为1，否则翻倍
            size_t new_cap = (capacity_ == 0) ? 1 : capacity_ * 2;
            reserve(new_cap);
        }
        data_[size_++] = value; // 在尾部插入元素
    }
    
    void reserve(size_t new_cap) {
        if (new_cap > capacity_) {
            T* new_data = new T[new_cap]; // 分配新内存
            // 复制旧数据到新内存
            for (size_t i = 0; i < size_; ++i) {
                new_data[i] = data_[i];
            }
            delete[] data_;      // 释放旧内存
            data_ = new_data;
            capacity_ = new_cap;
        }
    }
    //==========================================================


    //==========================================================
    // 步骤 3.1：拷贝构造函数
    Vector(const Vector& other) {
        data_ = new T[other.capacity_];
        size_ = other.size_;
        capacity_ = other.capacity_;
        for (size_t i = 0; i < size_; ++i) {
            data_[i] = other.data_[i]; // 深拷贝元素
        }
    }
    
    Vector& operator=(Vector other) { // 传值调用，直接复用拷贝构造
        swap(other);
        return *this;
    }
    
    // 步骤 3.2：拷贝赋值运算符（使用copy-and-swap惯用法）​
    void swap(Vector& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    Vector& operator=(Vector other) { // 传值调用，直接复用拷贝构造
        swap(other);
        return *this;
    }
    
    void swap(Vector& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }
    //==========================================================


    //==========================================================
    //步骤 3.3：移动构造函数和移动赋值
    // 移动构造函数
    Vector(Vector&& other) noexcept 
    : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
    }

    // 移动赋值运算符
    Vector& operator=(Vector&& other) noexcept {
    if (this != &other) {
        delete[] data_;
        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }
    return *this;
    }
    //==========================================================


    //==========================================================
    //4. 完善元素访问与迭代器
    //​步骤 4.1：operator[]和at()
    T& operator[](size_t index) {
        return data_[index];
    }
    
    const T& operator[](size_t index) const {
        return data_[index];
    }
    
    T& at(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("Index out of range");
        }
        return data_[index];
    }
    //==========================================================


    //==========================================================
    //步骤 4.2：迭代器支持
    using iterator = T*;
    using const_iterator = const T*;

    iterator begin() { return data_; }
    iterator end() { return data_ + size_; }
    const_iterator begin() const { return data_; }
    const_iterator end() const { return data_ + size_; }
    //==========================================================

    //==========================================================
    // 5. 高级操作：insert、erase和emplace_back
    // ​步骤 5.1：insert实现
    iterator insert(iterator pos, const T& value) {
        size_t offset = pos - begin();
        if (size_ >= capacity_) {
            reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        pos = begin() + offset; // 防止扩容后迭代器失效
        // 将pos后的元素后移
        for (auto it = end(); it > pos; --it) {
            *it = std::move(*(it - 1));
        }
        *pos = value;
        ++size_;
        return pos;
    }

    //步骤 5.2：erase实现
    iterator erase(iterator pos) {
        if (pos < begin() || pos >= end()) {
            throw std::out_of_range("Invalid iterator");
        }
        // 将pos后的元素前移
        std::move(pos + 1, end(), pos);
        --size_;
        return pos;
    }

    //步骤 5.3：emplace_back（完美转发）​
    template <typename... Args>
    void emplace_back(Args&&... args) {
        if (size_ >= capacity_) {
            reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        // 在尾部直接构造对象
        new(data_ + size_) T(std::forward<Args>(args)...);
        ++size_;
    }
    //==========================================================
    // 6. 边界处理与优化
    // ​步骤 6.1：pop_back和clear
    void pop_back() {
        if (size_ > 0) {
            --size_;
            data_[size_].~T(); // 显式调用析构函数（对非POD类型必要）
        }
    }
    
    void clear() {
        size_ = 0;
    }

    // 步骤 6.2：resize实现
    void resize(size_t new_size, const T& value = T()) {
        if (new_size > size_) {
            reserve(new_size);
            for (size_t i = size_; i < new_size; ++i) {
                data_[i] = value;
            }
        }
        size_ = new_size;
    }
    //==========================================================


    //==========================================================
    // 基础功能占位
    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }
    bool empty() const { return size_ == 0; }
};


// 步骤 2.3：测试基础功能
void test1_base(){
    Vector<int> v;
    v.push_back(1);
    v.push_back(2);
    assert(v.size() == 2);
    assert(v.capacity() == 2);
}

void test2_up(){
    Vector<int> v;
    v.push_back(1);
    v.emplace_back(2);
    assert(v.size() == 2);

    Vector<int> v2 = v; // 测试拷贝构造
    assert(v2[0] == 1);

    v.insert(v.begin(), 0);
    assert(v[0] == 0);

    v.erase(v.begin());
    assert(v[0] == 1);
}

int main(){
    test1_base();
    // test2_up();
    return 0;
}