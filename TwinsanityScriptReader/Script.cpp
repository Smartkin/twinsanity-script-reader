#include "Script.hpp"

// Statics
int Script::linkDepth = 0;
Script::MainScript::LinkedScriptsStruct** Script::linkedScriptList = new Script::MainScript::LinkedScriptsStruct*[256];

void Script::LoadLinkedScriptsData(MainScript::LinkedScriptsStruct* linked, std::ifstream& scriptFile)
{
	linkedScriptList[linkDepth++] = linked;
	linked->bitfield = ReadType<short>(scriptFile);
	linked->scriptIndexOrSlot = ReadType<short>(scriptFile);
	if ((linked->bitfield & 0x4000) != 0)
	{
		linked->support1 = new MainScript::ScriptSupport1();
		LoadScriptSupport1(linked->support1, scriptFile);
	}
	if ((linked->bitfield & 0x8000) != 0)
	{
		linked->next = new MainScript::LinkedScriptsStruct();
		LoadLinkedScriptsData(linked->next, scriptFile);
	}
}

void Script::LoadScriptSupport1(MainScript::ScriptSupport1* support1, std::ifstream& scriptFile)
{
	support1->unkByte1 = ReadType<char>(scriptFile);
	support1->unkByte2 = ReadType<char>(scriptFile);
	support1->unkUShort1 = ReadType<ushort>(scriptFile);
	support1->unkInt1 = ReadType<int>(scriptFile);
	size_t byteArrayLen = support1->unkByte1 + support1->unkByte2 * 4;
	support1->unkByteArray = new char[byteArrayLen];
	ReadType<char>(scriptFile, support1->unkByteArray, byteArrayLen);
}

void Script::LoadScriptSupport2(MainScript::LinkedScriptsStruct* linked, std::ifstream& scriptFile)
{
	if ((linked->bitfield & 0x1F) != 0)
	{
		linked->support2 = new MainScript::ScriptSupport2();
		LoadScriptSupport2Data(linked->support2, scriptFile);
	}
}

void Script::LoadScriptSupport2Data(MainScript::ScriptSupport2* support2, std::ifstream& scriptFile)
{
	support2->bitfield = ReadType<int>(scriptFile);
	if ((support2->bitfield & 0x400) != 0)
	{
		int linkedScriptListIndex = ReadType<int>(scriptFile);
		support2->linkedScript = linkedScriptList[linkedScriptListIndex];
	}
	if ((support2->bitfield & 0x200) != 0)
	{
		support2->support3 = new MainScript::ScriptSupport3();
		LoadScriptSupport3(support2->support3, scriptFile);
	}
	if ((support2->bitfield & 0xFF) != 0)
	{
		support2->support4 = new MainScript::ScriptSupport4();
		LoadScriptSupport4(support2->support4, scriptFile);
	}
	if ((support2->bitfield & 0x800) != 0)
	{
		MainScript::ScriptSupport2* nextSupport2 = new MainScript::ScriptSupport2();
		LoadScriptSupport2Data(nextSupport2, scriptFile);
		support2->next = nextSupport2;
	}
}

void Script::LoadScriptSupport3(MainScript::ScriptSupport3* support3, std::ifstream& scriptFile)
{
	support3->unkInt = ReadType<int>(scriptFile);
	support3->unkVtableInELF = 0x0;
	ReadType<char>(scriptFile, support3->unkBytes1, 4);
	ReadType<char>(scriptFile, support3->unkBytes2, 4);
	ReadType<char>(scriptFile, support3->unkBytes3, 4);
}

void Script::LoadScriptSupport4(MainScript::ScriptSupport4* support4, std::ifstream& scriptFile)
{
	int index = ReadType<int>(scriptFile);
	support4->internalIndex = index;
	support4->size = GetSupport4Size(index & 0x00FFFFFF);
	if (support4->size - 0xC > 0x0)
	{
		support4->unkByteArray = new char[support4->size - 0xC];
		ReadType<char>(scriptFile, support4->unkByteArray, support4->size - 0xC);
	}
	if ((index & 0x1000000) != 0)
	{
		MainScript::ScriptSupport4* nextSupport4 = new MainScript::ScriptSupport4();
		LoadScriptSupport4(nextSupport4, scriptFile);
		support4->next = nextSupport4;
		int flag = nextSupport4->size != 0 ? 1 : 0;
		support4->unkUInt = support4->unkUInt & 0xFEFFFFFF | flag << 0x18;
		return;
	}
	support4->unkUInt = support4->unkUInt & 0xFEFFFFFF;
}

int Script::GetSupport4Size(int support4Index)
{
	// Any that return 0 are technically undefined result and most likely get searched in some other tables
	switch (support4Index)
	{
	case 0x0:
		return 0x0;
	case 0x1:
		return 0x80;
	case 0x2:
		return 0xC;
	case 0x3:
		return 0x20;
	case 0x4:
		return 0x10;
	case 0x5:
		return 0xC;
	case 0x6:
		return 0x0;
	case 0x7:
		return 0xC;
	case 0x8:
		return 0x30;
	case 0x9:
		return 0x24;
	case 0xA:
		return 0x30;
	case 0xB:
		return 0x48;
	case 0xC:
		return 0x94;
	case 0xD:
		return 0xC;
	case 0xE:
		return 0x10;
	case 0xF:
		return 0x10;
	case 0x10:
		return 0x10;
	case 0x11:
		return 0x10;
	case 0x12:
		return 0x10;
	case 0x13:
		return 0x10;
	case 0x14:
	case 0x15:
	case 0x16:
		return 0x0;
	case 0x17:
		return 0x10;
	case 0x18:
		return 0x10;
	case 0x19:
	case 0x1A:
		return 0x0;
	case 0x1B:
		return 0x10;
	case 0x1C:
		return 0x10;
	case 0x1D:
		return 0x20;
	case 0x1E:
		return 0x0;
	case 0x1F:
		return 0x10;
	case 0x20:
		return 0x0;
	case 0x21:
		return 0x10;
	case 0x22:
		return 0x10;
	case 0x23:
		return 0xC;
	case 0x24:
		return 0xC;
	case 0x25:
	case 0x26:
		return 0x0;
	case 0x27:
		return 0xC;
	case 0x28:
		return 0x14;
	case 0x29:
		return 0x0;
	case 0x2A:
		return 0x10;
	case 0x2B:
		return 0x0;
	case 0x2C:
		return 0x50;
	case 0x2D:
		return 0x10;
	case 0x2E:
		return 0x0;
	case 0x2F:
		return 0x30;
	case 0x30:
		return 0x30;
	case 0x31:
		return 0x30;
	case 0x32:
		return 0xC;
	case 0x33:
		return 0x20;
	case 0x34:
		return 0xC;
	case 0x35:
		return 0xC;
	case 0x36:
		return 0x1C;
	case 0x72:
	case 0x73:
		return 0xC;
	case 0x75:
		return 0x14;
	case 0x8B:
		return 0x10;
	case 0x213:
		return 0xC;
	default:
		return 0x10; // Return 0x10 by default for now even though it's supposed to be 0x0 :P
	}
}
