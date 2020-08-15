#include "JSONWriter.hpp"

JSONWriter::JSONWriter(std::string filename)
{
	this->file.open(filename.c_str(), std::fstream::out);
	this->file.close();
	this->file.open(filename.c_str(), std::fstream::out | std::fstream::in);
	WriteType<char>("{", 1);
	keyClosed = true;
	depth = 1;
}

JSONWriter::~JSONWriter()
{
	this->file.seekp(0, this->file.end);
	WriteType<char>("\n}\n", 2);
	this->file.close();
}

void JSONWriter::write_key(std::string key)
{
	if (!keyClosed) throw std::exception("Key not closed by value!");
	curKey = key;
	keyClosed = false;
	WriteType<char>("\n", 1);
	for (size_t i = 0; i < depth; i++)
	{
		WriteType<char>("\t", 1);
	}
	WriteType<char>("\"", 1);
	WriteType<char>(key.c_str(), key.length());
	WriteType<char>("\": ", 3);
}

void JSONWriter::insert_key(JSONWriter::JSON_ObjectState state, std::string key)
{
	if (!keyClosed) throw std::exception("Key not closed by value!");
	curKey = std::string(key.c_str());
	keyClosed = false;
	InsertType(state.filePos, "\n", 1);
	state.filePos++;
	for (size_t i = 0; i < state.depth; i++)
	{
		InsertType(state.filePos, "\t", 1);
		state.filePos++;
	}
	InsertType(state.filePos, key.c_str(), key.length());
	state.filePos += key.length();
	InsertType(state.filePos, ": ", 2);
}

void JSONWriter::addKeyToObject(std::string objName, std::string key)
{
	JSON_ObjectState state = jsonMap.at(objName);
	insert_key(state, key);
}

void JSONWriter::start_object()
{
	bool addToMap = !keyClosed;
	keyClosed = true;
	WriteType<char>("\n", 1);
	for (size_t i = 0; i < depth; i++)
	{
		WriteType<char>("\t", 1);
	}
	depth++;
	WriteType<char>("{", 1);
	if (addToMap)
	{
		this->jsonMap.try_emplace(this->curKey, JSON_ObjectState(this->file.tellp(), depth));
	}
}

void JSONWriter::end_object()
{
	WriteType<char>("\n", 1);
	depth--;
	for (size_t i = 0; i < depth; i++)
	{
		WriteType<char>("\t", 1);
	}
	WriteType<char>("},", 2);
}

