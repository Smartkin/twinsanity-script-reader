#pragma once
#include <fstream>
#include "JSONWriter.hpp"

typedef unsigned short ushort;
typedef unsigned int uint;

std::string generate_key(std::string mainKey, int subKey);

class Script
{
public:
	struct HeaderScript
	{
		struct UnkIntPairs
		{
			uint mainScriptIndex;
			uint unkInt2;
		};
		uint unkIntPairsAmt;
		UnkIntPairs* pairs;
	};
	struct MainScript
	{
		struct LinkedScriptsStruct;
		struct ScriptSupport1;
		struct ScriptSupport2;
		struct ScriptSupport3;
		struct ScriptSupport4;

		LinkedScriptsStruct* firstLinkedScript;
		LinkedScriptsStruct* unkLinkedScript;

		struct ScriptSupport1
		{
			char unkByte1;
			char unkByte2;
			ushort unkUShort1;
			int unkInt1;
			char* unkByteArray;
		};
		struct ScriptSupport2
		{
			int bitfield;
			LinkedScriptsStruct* linkedScript;
			ScriptSupport3* support3;
			ScriptSupport4* support4;
			ScriptSupport2* next;
		};
		struct ScriptSupport3
		{
			int unkInt;
			int unkVtableInELF;
			char unkBytes1[4];
			char unkBytes2[4];
			char unkBytes3[4];
		};
		struct ScriptSupport4
		{
			uint unkUInt;
			int unkVtableInELF;
			int internalIndex;
			int size;
			char* unkByteArray;
			ScriptSupport4* next;
		};
		struct LinkedScriptsStruct
		{
			short bitfield;
			short scriptIndexOrSlot;
			ScriptSupport1* support1;
			ScriptSupport2* support2;
			LinkedScriptsStruct* next;
		};
	};

	short id;
	char mask;
	char flag;
	int nameLength;
	char* name;
	MainScript* mainScript;
	HeaderScript* headerScript;
	char* leftOverData;

private:
	static int linkDepth;
	static MainScript::LinkedScriptsStruct** linkedScriptList;

public:
	void LoadLinkedScriptsData(MainScript::LinkedScriptsStruct* linked, std::ifstream& scriptFile);
	void LoadScriptSupport1(MainScript::ScriptSupport1* support1, std::ifstream& scriptFile);
	void LoadScriptSupport2(MainScript::LinkedScriptsStruct* linked, std::ifstream& scriptFile);
	void LoadScriptSupport2Data(MainScript::ScriptSupport2* support2, std::ifstream& scriptFile);
	void LoadScriptSupport3(MainScript::ScriptSupport3* support3, std::ifstream& scriptFile);
	void LoadScriptSupport4(MainScript::ScriptSupport4* support4, std::ifstream& scriptFile);
	int GetSupport4Size(int support4Index);

	template<typename T>
	static T ReadType(std::ifstream& f);

	template<typename T>
	static void ReadType(std::ifstream& f, T* data, size_t size);
};

template<typename T>
T Script::ReadType(std::ifstream& f)
{
	T obj;
	f.read(reinterpret_cast<char*>(&obj), sizeof(obj));
	return obj;
}

template<typename T>
void Script::ReadType(std::ifstream& f, T* data, size_t size)
{
	f.read(reinterpret_cast<char*>(data), size);
}

// Global script dump
extern JSONWriter scriptDump;