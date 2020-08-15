// TwinsanityScriptReader.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include "Script.hpp"
#include "JSONWriter.hpp"

JSONWriter scriptDump("ScriptDump.json");

void SerializeLinkedScripts(Script::MainScript::LinkedScriptsStruct* linked, bool ignoreSupport2 = false);
void SerializeSupport1(Script::MainScript::ScriptSupport1* support1);
void SerializeSupport2(Script::MainScript::ScriptSupport2* support2);
void SerializeSupport3(Script::MainScript::ScriptSupport3* support3);
void SerializeSupport4(Script::MainScript::ScriptSupport4* support4);

void ReadScript(std::ifstream& scriptFile)
{
	Script script;
	script.id = Script::ReadType<short>(scriptFile);
	script.mask = Script::ReadType<char>(scriptFile);
	script.flag = Script::ReadType<char>(scriptFile);
	// Serialize the header of the script
	scriptDump.write("id", script.id);
	scriptDump.write("mask", script.mask);
	scriptDump.write("flag", script.flag);
	if (script.flag == '\0')
	{
		script.nameLength = Script::ReadType<int>(scriptFile);
		script.name = new char[script.nameLength + 1];
		Script::ReadType<char>(scriptFile, script.name, script.nameLength);
		script.name[script.nameLength] = '\0';
		scriptDump.write("Name_length", script.nameLength);
		scriptDump.write("Name", std::string(script.name));
		int unkInt1 = Script::ReadType<int>(scriptFile);
		int unkInt2 = Script::ReadType<int>(scriptFile);
		scriptDump.write("unkInt1", unkInt1);
		scriptDump.write("unkInt2", unkInt2);
		script.mainScript = new Script::MainScript();
		int counter = 0;
		script.mainScript->firstLinkedScript = new Script::MainScript::LinkedScriptsStruct();
		script.LoadLinkedScriptsData(script.mainScript->firstLinkedScript, scriptFile);
		std::cout << "File position: " << scriptFile.tellg() << "\n";
		Script::MainScript::LinkedScriptsStruct* linked = script.mainScript->firstLinkedScript;
		while (linked != nullptr)
		{
			script.LoadScriptSupport2(linked, scriptFile);
			if (counter++ == unkInt2)
			{
				script.mainScript->unkLinkedScript = linked;
			}
			linked = linked->next;
		}
		// Serialize the main script
		scriptDump.write_key("Main_Script");
		scriptDump.start_object();
		SerializeLinkedScripts(script.mainScript->firstLinkedScript);
		//SerializeLinkedScripts(script.mainScript->unkLinkedScript); Potentially used when actually executing the script
		scriptDump.end_object();
		std::cout << "Final offset position: " << scriptFile.tellg() << "\n";
	}
	else
	{
		script.nameLength = sizeof("Header script");
		script.name = new char[script.nameLength];
		script.name = const_cast<char*>("Header script");
		script.headerScript = new Script::HeaderScript();
		script.headerScript->unkIntPairsAmt = Script::ReadType<uint>(scriptFile);
		script.headerScript->pairs = new Script::HeaderScript::UnkIntPairs[script.headerScript->unkIntPairsAmt];
		for (int i = 0; i < script.headerScript->unkIntPairsAmt; i++)
		{
			script.headerScript->pairs[i].mainScriptIndex = Script::ReadType<uint>(scriptFile);
			script.headerScript->pairs[i].unkInt2 = Script::ReadType<uint>(scriptFile);
		}
		// Serialize the header script
		scriptDump.write("Name_length", script.nameLength);
		scriptDump.write("Name", std::string(script.name));
		scriptDump.write("unkIntPairsAmt", script.headerScript->unkIntPairsAmt);
		for (int i = 0; i < script.headerScript->unkIntPairsAmt; i++)
		{
			scriptDump.write_key("Pair");
			scriptDump.start_object();
			scriptDump.write("mainScriptIndex", script.headerScript->pairs[i].mainScriptIndex);
			scriptDump.write("unkInt2", script.headerScript->pairs[i].unkInt2);
			scriptDump.end_object();
		}
	}
	auto pos = scriptFile.tellg();
	scriptFile.seekg(0, scriptFile.end);
	if (scriptFile.tellg() != pos)
	{
		scriptFile.seekg(pos, scriptFile.beg);
		auto curPosition = scriptFile.tellg();
		scriptFile.seekg(0, scriptFile.end);
		int leftOverSize = scriptFile.tellg() - curPosition;
		scriptFile.seekg(curPosition, scriptFile.beg);
		script.leftOverData = new char[leftOverSize];
		Script::ReadType<char>(scriptFile, script.leftOverData, leftOverSize);
		std::cout << "Leftover bytes: ";
		for (size_t i = 0; i < leftOverSize; i++)
		{
			std::cout << std::hex << (int)script.leftOverData[i];
		}
		std::cout << "\n";
	}
	std::cout << "Finished reading script!\n";
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "No script file provided!\n";
		return 1;
	}
	std::ifstream scriptFile;
	scriptFile.open(argv[1], std::ifstream::binary | std::ifstream::in);
	
	if (!scriptFile.is_open())
	{
		std::cout << "Failed to open specified file!";
		return 1;
	}
	ReadScript(scriptFile);
}


void SerializeLinkedScripts(Script::MainScript::LinkedScriptsStruct* linked, bool ignoreSupport2)
{
	scriptDump.write_key("LinkedScript");
	scriptDump.start_object();
	scriptDump.write("bitfield", linked->bitfield);
	if ((linked->bitfield & 0x1000) != 0) {
		scriptDump.write("Object_scripts_table_slot_index", linked->scriptIndexOrSlot);
	}
	else
	{
		scriptDump.write("Script_id", linked->scriptIndexOrSlot);
	}
	if ((linked->bitfield & 0x4000) != 0)
	{
		SerializeSupport1(linked->support1);
	}
	else
	{
		scriptDump.write("support1", std::string("NULL"));
	}
	if (!ignoreSupport2)
	{
		if ((linked->bitfield & 0x1F) != 0)
		{
			SerializeSupport2(linked->support2);
		}
		else
		{
			scriptDump.write("support2", std::string("NULL"));
		}
	}
	if ((linked->bitfield & 0x8000) != 0)
	{
		SerializeLinkedScripts(linked->next, ignoreSupport2);
	}
	else
	{
		scriptDump.write("LinkedScript", std::string("NULL"));
	}
	scriptDump.end_object();
}

void SerializeSupport1(Script::MainScript::ScriptSupport1* support1)
{
	scriptDump.write_key("support1");
	scriptDump.start_object();
	scriptDump.write("unkByte1", support1->unkByte1);
	scriptDump.write("unkByte2", support1->unkByte2);
	scriptDump.write("unkUShort1", support1->unkUShort1);
	scriptDump.write("unkInt1", support1->unkInt1);
	size_t byteArrayLen = support1->unkByte1 + support1->unkByte2 * 4;
	std::stringstream byteArrayStr;
	byteArrayStr << "0x" << std::setfill('0') << std::setw(sizeof(char) * 2);
	for (size_t i = 0; i < byteArrayLen; i++)
	{
		byteArrayStr << std::hex << (int)support1->unkByteArray[i];
	}
	scriptDump.write<std::string>("unkByteArray", byteArrayStr.str());
	scriptDump.end_object();
}

void SerializeSupport2(Script::MainScript::ScriptSupport2* support2)
{
	scriptDump.write_key("support2");
	scriptDump.start_object();
	scriptDump.write("bitfield", support2->bitfield);
	if ((support2->bitfield & 0x400) != 0)
	{
		SerializeLinkedScripts(support2->linkedScript, true);
	}
	else
	{
		scriptDump.write("LinkedScript", std::string("NULL"));
	}
	if ((support2->bitfield & 0x200) != 0)
	{
		SerializeSupport3(support2->support3);
	}
	else
	{
		scriptDump.write("support3", std::string("NULL"));
	}
	if ((support2->bitfield & 0xFF) != 0)
	{
		SerializeSupport4(support2->support4);
	}
	else
	{
		scriptDump.write("support4", std::string("NULL"));
	}
	if ((support2->bitfield & 0x800) != 0)
	{
		SerializeSupport2(support2->next);
	}
	else
	{
		scriptDump.write("support2", std::string("NULL"));
	}
	scriptDump.end_object();
}

void SerializeSupport3(Script::MainScript::ScriptSupport3* support3)
{
	scriptDump.write_key("support3");
	scriptDump.start_object();
	scriptDump.write("unkInt", support3->unkInt);
	scriptDump.write("unkVTableInELF", support3->unkVtableInELF);
	std::stringstream byteArrayStr1;
	byteArrayStr1 << "0x" << std::setfill('0') << std::setw(sizeof(char) * 2);
	for (size_t i = 0; i < 4; i++)
	{
		byteArrayStr1 << std::hex << (int)support3->unkBytes1[i];
	}
	scriptDump.write<std::string>("unkBytes1", byteArrayStr1.str());
	std::stringstream byteArrayStr2;
	byteArrayStr2 << "0x" << std::setfill('0') << std::setw(sizeof(char) * 2);
	for (size_t i = 0; i < 4; i++)
	{
		byteArrayStr2 << std::hex << (int)support3->unkBytes2[i];
	}
	scriptDump.write<std::string>("unkBytes2", byteArrayStr2.str());
	std::stringstream byteArrayStr3;
	byteArrayStr3 << "0x" << std::setfill('0') << std::setw(sizeof(char) * 2);
	for (size_t i = 0; i < 4; i++)
	{
		byteArrayStr3 << std::hex << (int)support3->unkBytes3[i];
	}
	scriptDump.write<std::string>("unkBytes3", byteArrayStr3.str());
	scriptDump.end_object();
}

void SerializeSupport4(Script::MainScript::ScriptSupport4* support4)
{
	scriptDump.write_key("support4");
	scriptDump.start_object();
	scriptDump.write("Internal_index", support4->internalIndex);
	scriptDump.write("size", support4->size);
	if (support4->size - 0xC > 0x0)
	{
		size_t byteArrayLen = support4->size - 0xC;
		std::stringstream byteArrayStr;
		byteArrayStr << "0x" << std::setfill('0') << std::setw(sizeof(char) * 2);
		for (size_t i = 0; i < byteArrayLen; i++)
		{
			byteArrayStr << std::hex << (int)support4->unkByteArray[i];
		}
		scriptDump.write<std::string>("unkByteArray", byteArrayStr.str());
	}
	else
	{
		scriptDump.write("unkByteArray", std::string("NULL"));
	}
	if (support4->next != NULL)
	{
		SerializeSupport4(support4->next);
	}
	else
	{
		scriptDump.write("support4", std::string("NULL"));
	}
	scriptDump.write("unkUInt", support4->unkUInt);
	scriptDump.end_object();
}
