#include "table.h"
#include "common.h"

/*
获取数组部分长度
	1: 只适用于整数key连续，无空洞的情况
	2: 整数key有可能在hash部分 
*/
int Table::GetN()
{
	if (array_size_ > 0 && IS_NIL(array_datas_[array_size_ - 1])) // 最后一个元素为nil，二分查找数组
	{
		int i = 0;
		int j = array_size_ - 1;
		while (i < j)
		{
			int m = (i + j) / 2;
			if (NOT_NIL(array_datas_[m]))
			{
				if (IS_NIL(array_datas_[m + 1]))
					return m + 1;
				else
					i = m + 1;
			}
			else
				j = m;
		}
		return 0;
	}
	if (hash_size_ == 0)
		return array_size_;

	int i = array_size_ - 1;
	int j = array_size_;
	while (NOT_NIL(GetNum(j))) // 数组最后一个元素不为nil，继续查找hash部分的整数key，从 arraySize开始，翻倍检查整数key是否存在
	{
		i = j;
		j *= 2;
	}

	while (i < j) // 二分查找最后一个整数key
	{
		int mpos = (i + j) / 2;
		if (NOT_NIL(GetNum(mpos)))
		{
			if (IS_NIL(GetNum(mpos + 1)))
				return mpos;
			else
				i = mpos + 1;
		}
		else
			j = mpos;
	}

	return 0;
}

TObject Table::NextKey(const TObject& key)
{
	TObject next_key;
	int pos = GetPos(key);
	if (pos == -1)
		return next_key;

	++pos;
	for (; pos < array_size_; ++pos) // 下一个数组key
	{
		if (NOT_NIL(array_datas_[pos - 1]))
		{
			next_key.AsNumber(pos);
			return next_key;
		}
	}

	pos -= array_size_;
	for (; pos < hash_size_; ++pos) // 下一个哈希key
	{
		if (hash_datas_[pos - 1].IsUsed())
			return hash_datas_[pos - 1].key_;
	}

	return next_key;
}

void Table::Set(const TObject& key, const TObject& value)
{
	if (IS_NIL(key))
		return;
	int n = NumberKey(key);
	if (n >= 1 && n <= array_size_)
	{
		array_datas_[n - 1] = value;
		return;
	}
	Node* node = SearchNode(key); // exist key
	if (node)
	{
		node->value_ = value;
		return;
	}

	Node* main_pos = MainPosition(key);
	if (main_pos->IsUsed())
	{
		Node* free_pos = GetLastFreePos();
		if (free_pos != nullptr)
		{
			Rehash(key);
			Set(key, value);
			return;
		}
		main_pos = AdjustPos(main_pos, free_pos, MainPosition(main_pos->key_));
	}
	else
	{
		main_pos->next_ = nullptr;
	}
	main_pos->key_ = key;
	main_pos->value_ = value;
}

void Table::SetNum(int n, const TObject& value)
{
	TObject key;
	key.AsNumber(n);
	Set(key, value);
}

TObject Table::Get(const TObject& key)
{
	int n = NumberKey(key);
	if (n > 1 && n <= array_size_)
	{
		return array_datas_[n - 1];
	}
	Node* node = SearchNode(key);
	return node ? node->value_ : TObject();
}

TObject Table::GetNum(int n)
{
	TObject key;
	key.AsNumber(n);
	return Get(key);
}

int Table::NumberKey(const TObject& key)
{
	return (key.object_type_ == ObjectType_Number && IS_INTEGER(key) ? (int)NUM_VAL(key) : -1);
}

Node* Table::SearchNode(const TObject& key)
{
	if (key.object_type_ != ObjectType_NIL)
	{
		Node* pos = MainPosition(key);
		while (pos)
		{
			if (pos->key_.Equal(key))
				return pos;
			pos = pos->next_;
		}
	}
	return nullptr;
}

Node* Table::MainPosition(const TObject& key)
{
	HashNum hash_num = 0;
	switch (key.object_type_)
	{
	case ObjectType_Number:
		hash_num = BaseFunc::GenNumHash(NUM_VAL(key));
		break;
	case ObjectType_CProto:
		hash_num = (HashNum)(long)key.value_.c_func_;
		break;
	case ObjectType_String:
		hash_num = key.hash_num_;
		break;
	case ObjectType_Table:
		hash_num = (HashNum)(long)key.value_.proto_;
		break;
	case ObjectType_Proto:
		hash_num = (HashNum)(long)key.value_.proto_;
		break;
	default:
		break;
	}
	return &hash_datas_[MOD_VAL(hash_num, hash_size_)];
}

Node* Table::GetLastFreePos()
{
	while (last_free_)
	{
		if (!hash_datas_[last_free_].IsUsed())
			return &hash_datas_[last_free_];
		--last_free_;
	}
	return NULL;
}

/* 
处理冲突结点: 将之前冲突节点放到此处的节点
	mainPos：新结点的主位置
	freePos：空闲结点位置
	actualPos：mainPos位置上已有结点的实际主位置
*/
Node* Table::AdjustPos(Node* main_pos, Node* free_pos, Node* actual_pos)
{
	if (actual_pos == main_pos) // 已有节点在正确位置上，新节点头部插入冲突链
	{
		free_pos->next_ = main_pos->next_;
		main_pos->next_ = free_pos;
		return free_pos;
	}

	while (main_pos != actual_pos->next_) // 已有节点在错误位置，获取器实际主位置的前置节点
	{
		actual_pos = actual_pos->next_;
	}
	*free_pos = *main_pos; // 将位置错误节点放到新的空位置上
	actual_pos->next_ = free_pos; // 前置节点指向free_pos
	main_pos->next_ = nullptr; // 正确节点清空

	return main_pos;
}

/* 
计算位置
	1: 数组从 1 到 array_size_
	2: 哈希从 array_size_ + 1 到 array_size_ + hash_size_
*/
int Table::GetPos(const TObject& key)
{
	if (IS_NIL(key))
		return 0;
	int n = NumberKey(key);
	if (n >= 1 && n < array_size_)
		return n;
	Node* node = SearchNode(key);
	if (node)
		return (node - hash_datas_) + array_size_ + 1;
	return -1;
}

void Table::Rehash(const TObject& key)
{
	int sections[INT_BITS + 1] = { 0 };
	int number_key_num = 0;
	int key_num = 1; // 新插入触发rehash，多一个，所以初始值为1

	ArrayStat(sections, number_key_num, key_num); // 获取所有数字key和所有key的总数
	HashStat(sections, number_key_num, key_num);
	number_key_num += CountIntKey(key, sections) ? 1 : 0;

	number_key_num = ComputeArraySize(sections, number_key_num);
	Resize(number_key_num, key_num - number_key_num);
}

void Table::ArrayStat(int sections[], int& nkeys, int& keys)
{
	int cur_pos = 1;
	for (int i = 0, square_i = 1; i <= INT_BITS && cur_pos <= array_size_; ++i, square_i *= 2) // ( 2^(i-1), 2^i ] 为一个区间
	{
		int maxn = MIN_VAL(square_i, array_size_);
		int count = 0;
		for (; cur_pos <= maxn; ++cur_pos)
		{
			count += IS_NIL(array_datas_[cur_pos - 1]) ? 0 : 1;
		}
		sections[i] += count;
		nkeys += count;
	}
	keys += nkeys;
}

void Table::HashStat(int sections[], int& nkeys, int& keys)
{
	for (int i = 0; i < hash_size_; ++i)
	{
		if (hash_datas_[i].IsUsed())
		{
			nkeys += CountIntKey(hash_datas_[i].key_, sections) ? 1 : 0;
			++keys;
		}
	}
}

bool Table::CountIntKey(const TObject& key, int sections[])
{
	int n = NumberKey(key);
	if (n != -1)
	{
		sections[BaseFunc::CeilLog2(n)]++;
		return true;
	}
	return false;
}

int Table::ComputeArraySize(int sections[], int nkeys)
{
	int count = 0;
	int array_size = 0;
	for (int i = 0, square_i = 1; i <= INT_BITS; ++i, square_i *= 2)
	{
		if (sections[i] > 0) 
		{
			count += sections[i];
			if (count > square_i / 2) // 数组使用率超过50%
			{
				array_size = count;
			}
		}
		if (count == nkeys)
			break;
	}
	return array_size;
}

void Table::Resize(int array_size, int hash_size)
{
	TObject* old_array = array_datas_;
	int old_array_size = array_size_;
	Node* old_hash = hash_datas_;
	int old_hash_size = hash_size_;

	array_size_ = BaseFunc::SquareSize(array_size, INIT_SIZE_TABLE_ARRAY);
	hash_size_ = BaseFunc::SquareSize(hash_size, INIT_SIZE_TABLE_HASH);
	hash_datas_ = Allocator<Node>::NewArray(hash_size_, FUNC_NAME, FUNC_NAME);
	last_free_ = hash_size_ - 1;

	if (array_size_ != old_array_size) // 重新分配数组
	{
		array_datas_ = Allocator<TObject>::NewArray(array_size_, FUNC_NAME, FUNC_NAME);
		::memcpy(array_datas_, old_array, MIN_VAL(array_size_, old_array_size) * sizeof(TObject));

		for (int i = array_size_; i < old_array_size; ++i) // 收缩数组， 多余的整数key存入hash
		{
			if (NOT_NIL(old_array[i]))
				SetNum(i, old_array[i]);
		}
		Allocator<TObject>::DelArray(old_array);
	}

	for (int i = old_hash_size - 1; i >= 0; --i)
	{
		if (old_hash[i].IsUsed())
			Set(old_hash[i].key_, old_hash[i].value_);
	}
	Allocator<Node>::DelArray(old_hash);
}
