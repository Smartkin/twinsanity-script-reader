template<typename T>
inline std::string JSONWriter::ValueToHex(T i)
{
	std::stringstream stream;
	stream << "0x"
		<< std::setfill('0') << std::setw(sizeof(T) * 2)
		<< std::hex << i;
	return stream.str();
}

template<>
inline std::string JSONWriter::ValueToHex<char>(char i)
{
	std::stringstream stream;
	stream << "0x"
		<< std::setfill('0') << std::setw(sizeof(char) * 2)
		<< std::hex << (int)i;
	return stream.str();
}

template<typename T>
inline void JSONWriter::write(std::string key, const T value)
{
	write_key(key);
	write_value(value);
}

template<typename T>
inline void JSONWriter::write_value(const T value)
{
	if (keyClosed) throw std::exception("Key not opened for value!");
	keyClosed = true;
	std::string str = ValueToHex(value);
	WriteType<char>(str.c_str(), str.length());
	WriteType<char>(",", 1);
}

template<typename T>
inline void JSONWriter::insert_value(const T value)
{
	if (keyClosed) throw std::exception("Key not opened for value!");
	keyClosed = true;
	std::string str = ValueToHex(value);
	InsertType(file.tellp(), str.c_str(), str.length());
	InsertType(file.tellp(), ",", 1);
}

template<typename T>
inline void JSONWriter::addToObject(std::string objName, std::string key, const T value)
{
	addKeyToObject(objName, key);
	insert_value(value);
}

template<>
inline void JSONWriter::write_value<std::string>(const std::string value)
{
	if (keyClosed) throw std::exception("Key not opened for value!");
	keyClosed = true;
	std::string str = value;
	WriteType("\"", 1);
	WriteType<char>(str.c_str(), str.length());
	WriteType("\"", 1);
	WriteType<char>(",", 1);
}

template<>
inline void JSONWriter::insert_value<std::string>(const std::string value)
{
	if (keyClosed) throw std::exception("Key not opened for value!");
	keyClosed = true;
	std::string str = value;
	InsertType(file.tellp(), "\"", 1);
	InsertType(file.tellp(), str.c_str(), str.length());
	InsertType(file.tellp(), "\"", 1);
	InsertType(file.tellp(), ",", 1);
}

template<>
inline void JSONWriter::write_value<char>(const char value)
{
	if (keyClosed) throw std::exception("Key not opened for value!");
	keyClosed = true;
	std::string str = ValueToHex(value);
	WriteType<char>(str.c_str(), str.length());
	WriteType<char>(",", 1);
}

template<typename T>
inline void JSONWriter::WriteType(const T& data)
{
	file.write(reinterpret_cast<const char*>(&data), sizeof(data));
}

template<typename T>
inline void JSONWriter::WriteType(const T* data, size_t size)
{
	file.write(reinterpret_cast<const char*>(data), size);
	if (!file.is_open() || file.bad()) throw std::exception("File is not opened or stream got corrupt after writing");
}

template<typename T>
inline void JSONWriter::InsertType(const std::streampos pos, const T& data)
{
	if (!file.is_open() || file.bad()) throw std::exception("File is not opened or stream got corrupt after writing");
	file.flush();
	file.seekp(0, file.end);
	auto filePos = file.tellp();
	size_t moveDataAmount = filePos - pos;
	char* moveData = new char[moveDataAmount];
	file.seekp(pos, file.beg);
	file.read(moveData, moveDataAmount);
	file.flush();
	file.clear();
	file.seekp(pos, file.beg);
	WriteType(data);
	WriteType(moveData, moveDataAmount);
}

template<typename T>
inline void JSONWriter::InsertType(const std::streampos pos, const T* data, size_t size)
{
	if (!file.is_open() || file.bad()) throw std::exception("File is not opened or stream got corrupt after writing");
	file.flush();
	file.seekp(0, file.end);
	auto filePos = file.tellp();
	size_t moveDataAmount = filePos - pos;
	char* moveData = new char[moveDataAmount];
	file.seekp(pos, file.beg);
	file.read(moveData, moveDataAmount);
	file.flush();
	file.clear();
	file.seekp(pos, file.beg);
	WriteType(data, size);
	WriteType(moveData, moveDataAmount);
}
