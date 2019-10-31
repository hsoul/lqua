#pragma once

#include <cstring>
#include <cstdarg>
#include <cstdio>

#define array_grow(arr) { if (arr.size_ >= arr.capacity_) { arr.Grow(__FUNCTION__); } }

class BlockMonitor
{
public:
	static void Reset();
	static void Add(void* ptr, int size, const char* func, const char* pre_func);
	static void Del(void* ptr);
	static void Dump();
};

template<class T>
class Allocator
{
public:
	static T* New(const char* func, const char* pre_func)
	{
		T* obj = new T();
		BlockMonitor::Add(obj, sizeof(T), func, pre_func);
		return obj;
	}
	static T* NewArray(int size, const char* func, const char* pre_func)
	{
		T* obj = new T[size];
		BlockMonitor::Add(obj, size, func, pre_func);
		return obj;
	}
	static void Del(T*& ptr)
	{
		if (ptr)
		{
			BlockMonitor::Del(ptr);
			delete ptr;
			ptr = nullptr;
		}
	}
	static void DelArray(T*& ptr)
	{
		if (ptr)
		{
			BlockMonitor::Del(ptr);
			delete[] ptr;
			ptr = nullptr;
		}
	}
};

template<class T>
class Array
{
public:
	Array()
		: datas_(nullptr)
		, capacity_(0)
		, size_(0)
	{

	}
	Array(const Array&) = delete;  /*禁用复制构造，如 Array a=b; */
	Array& operator=(const Array&) = delete;  /*禁用赋值操作，如 Array a; a=b; */
	~Array()
	{
		Allocator<T>::DelArray(datas_);
	}
	void Init(int capacity, const char* pre_func)
	{
		datas_ = Allocator<T>::NewArray(capacity, __FUNCTION__, pre_func);
		capacity_ = capacity;
		size_ = 0;
	}
	void Grow(const char* pre_func)
	{
		int new_size = capacity_ * 2;
		T* new_datas = Allocator<T>::NewArray(new_size, __FUNCTION__, pre_func);
		::memcpy(new_datas, datas_, capacity_ * sizeof(T));
		Allocator<T>::DelArray(datas_);
		capacity_ = new_size;
		datas_ = new_datas;
	}
	void Add(T obj)
	{
		if (size_ >= capacity_)
			Grow(__FUNCTION__);
		datas_[size_++] = obj;
	}
	void clear() 
	{
		size_ = 0;
		Allocator<T>::DelArray(datas_);
		datas_ = Allocator<T>::NewArray(init_capacity_, __FUNCTION__, "");
		capacity_ = init_capacity_;
	}
	int size() const { return size_; }
	T& operator[](int index) const
	{
		return datas_[index];
	}
public:
	T* datas_;
	int init_capacity_;
	int capacity_;
	int size_;
};