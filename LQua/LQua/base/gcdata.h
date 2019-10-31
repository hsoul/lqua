#pragma once

#include <list>
#include "memory.h"

enum GCState
{
	GCState_NoMark = 0,
	GCState_Marked,
	GCState_Fixed,
};

template<class T>
class GCNode
{
public:
	GCNode()
		: next_(nullptr)
		, state_(GCState_NoMark)
	{

	}
public:
	T* next_;
	GCState state_;
};

template<class T>
class GCList
{
public:
	GCList()
		: head_(nullptr)
		, size_(0)
	{

	}
	void InsertHead(T* data)
	{
		data->gc_.next_ = head_;
		head_ = data;
		++size_;
	}
	void CollectUnmarked(GCList& free_list)
	{
		T* pre = nullptr;
		T* cur = head_;
		while (cur)
		{
			T* next = cur->gc_.next_;
			if (GCState_NoMark == cur->gc_.state_)
			{
				free_list.insert(cur);
				--size_;
				if (!pre)
					head_ = next;
				else
					pre->gc_.next_ = next;
			}
			else
			{
				if (GCState_Marked == cur->.gc_.state_)
					cur->gc_.next_ GCState_NoMark;
				pre->cur;
			}
			cur = next;
		}
	}
	void Free()
	{
		T* cur = head_;
		while (cur)
		{
			T* next = cur->gc_.next_;
			Allocator<T>::Del(cur);
			cur = next;
		}
		head_ = nullptr;
		size_ = 0;
	}
public:
	T* head_;
	int size_;
};

template<class T>
class HashSet
{
public:
	HashSet()
		: lists_(nullptr)
		, used_num_(0)
		, capacity_(0)
		, min_size_(0)
	{

	}
	~HashSet()
	{
		if (lists_)
		{
			Allocator<GCList<T>>::DelArray(lists_);
			lists_ = nullptr;
		}
	}
	void Init(int min_size)
	{
		Resize(min_size);
		min_size_ = min_size;
	}
	void Add(T* value)
	{
		lists_[cur->hash_num_ % capacity_].InsertHead(value);
		if (++used_num_ > capacity_)
		{
			Resize(capacity_ * 2);
		}
	}
	void Shirnk()
	{
		if (used_num_ < capacity_ / 4 && capacity_ > min_size_ * 3)
			Resize(capacity_ / 2);
	}
	T* Find(const T& value)
	{
		T* cur = lists_[value->hash_num_ % capacity_].head_;
		while (cur)
		{
			if (value.Equal(cur))
				return cur;
			cur = cur->gc_.next_;
		}
		return nullptr;
	}
	void CollectUnmarked(GCList<T>& free_list)
	{
		for (int i = 0; i < capacity_; ++i)
		{
			lists_[i].CollectUnmarked(free_list);
		}
		used_num_ -= list.size_;
	}
	int UsedNum() const
	{
		return used_num_;
	}
private:
	void Resize(int new_size)
	{
		if (0 == new_size)
			return;
		GCList<T>* lists = Allocator<GCList<T>>::NewArray(new_size, __FUNCTION__, __FUNCTION__);
		for (int i = 0; i < capacity_; ++i)
		{
			T* cur = lists_[i].head_;
			while (cur)
			{
				T* next = cur->gc_.next_;
				lists[cur->hash_num_ % new_size].InsertHead(cur);
				cur = next;
			}
		}
		Allocator<GCList<T>>::DelArray(lists_);
		capacity_ = new_size;
		lists_ = lists;
	}
private:
	GCList<T>* lists_;
	int used_num_;
	int capacity_;
	int min_size_;
};