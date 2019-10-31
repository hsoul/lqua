#pragma once

#include "memory.h"
#include "basedef.h"
#include "lobject.h"

class Table
{
private:
	struct Node
	{
	public:
		Node()
			: next_(nullptr)
		{

		}
		bool IsUsed()
		{
			return NOT_NIL(value_) && NOT_NIL(key_);
		}
	public:
		TObject key_;
		TObject value_;
		Node* next_;
	};
	enum { INT_BITS = 26, };
public:
	Table()
		: array_datas_(nullptr)
		, array_size_(0)
		, hash_datas_(nullptr)
		, hash_size_(0)
		, last_free_(-1)
	{

	}
	~Table()
	{
		Allocator<TObject>::DelArray(array_datas_);
		Allocator<Node>::DelArray(hash_datas_);
	}
	void Init(int array_size, int hash_size) { Resize(array_size, hash_size); }
private:
	void Resize(int array_size, int hash_size);
private:
	TObject* array_datas_;
	int array_size_;
	Node* hash_datas_;
	int hash_size_;
	int last_free_;
};