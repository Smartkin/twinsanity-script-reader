#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <map>

// Very sloppy/lazily made serializer
class JSONWriter
{
private:
	struct JSON_ObjectState
	{
		JSON_ObjectState(std::streamoff pos, int depth) : filePos(pos), depth(depth)
		{}

		std::streamoff filePos;
		int depth;
	};

public:
	JSONWriter(std::string filename);
	~JSONWriter();

	template<typename T>
	void write(std::string key, const T value);

	void write_key(std::string key);
	template<typename T>
	void write_value(const T value);

	void insert_key(JSON_ObjectState state, std::string key);
	template<typename T>
	void insert_value(const T value);

	template<typename T>
	void addToObject(std::string objName, std::string key, const T value);
	void addKeyToObject(std::string objName, std::string key);

	void start_object();
	void end_object();

private:

	template<typename T>
	void WriteType(const T& data);

	template<typename T>
	void WriteType(const T* data, size_t size);

	template<typename T>
	void InsertType(const std::streampos pos, const T& data);

	template<typename T>
	void InsertType(const std::streampos pos, const T* data, size_t size);

	template<typename T>
	std::string ValueToHex(T i);

	std::fstream file;

	std::map<std::string, JSON_ObjectState> jsonMap;
	int depth;
	bool keyClosed;
	std::string curKey;
};


#include "JSONWriter.inl"