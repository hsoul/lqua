#pragma once

class UseTime
{
public:
	UseTime(const char* name);
	~UseTime();
private:
	const char* name_;
	int start_time_;
};

class Test
{
public:
	static void TestArray();
};