#pragma once

#include "memory.h"
#include "basedef.h"
#include "lobject.h"

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

class Table
{
private:
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
	int GetN();
	TObject NextKey(const TObject& key);
	void Set(const TObject& key, const TObject& value);
	void SetNum(int n, const TObject& value);
	TObject Get(const TObject& key);
	TObject GetNum(int n);
private:
	int NumberKey(const TObject& key);
	Node* SearchNode(const TObject& key);
	Node* MainPosition(const TObject& key);
	Node* GetLastFreePos();
	Node* AdjustPos(Node* main_pos, Node* free_pos, Node* actual_pos);
	int GetPos(const TObject& key);
	void Rehash(const TObject& key);
	void ArrayStat(int sections[], int& nkeys, int& keys);
	void HashStat(int sections[], int& nkeys, int& keys);
	bool CountIntKey(const TObject& key, int sections[]);
	int ComputeArraySize(int sections[], int nkeys);
	void Resize(int array_size, int hash_size);
private:
	TObject* array_datas_;
	int array_size_;
	Node* hash_datas_;
	int hash_size_;
	int last_free_;
};