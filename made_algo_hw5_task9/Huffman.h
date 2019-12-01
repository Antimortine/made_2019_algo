#pragma once

#include <string>
#include <fstream>
#include <iterator>
#include <vector>

typedef unsigned char byte;

struct IInputStream
{
	explicit IInputStream(const std::string& path);

	// Возвращает false, если поток закончился
	bool Read(byte& value);

	std::vector<char> bytes;
	size_t position = 0;
};

struct IOutputStream
{
	explicit IOutputStream(std::string path) : path(std::move(path))
	{
	}

	void Write(byte value);
	void Flush();

	std::vector<char> bytes;
	std::string path;
};

inline IInputStream::IInputStream(const std::string& path)
{
	std::ifstream input(path, std::ios::binary);
	bytes = std::vector<char>(
		std::istreambuf_iterator<char>(input),
		std::istreambuf_iterator<char>());
	input.close();
}

inline bool IInputStream::Read(byte& value)
{
	if (position == bytes.size())
		return false;
	value = static_cast<byte>(bytes[position++]);
	return true;
}

inline void IOutputStream::Write(byte value)
{
	bytes.push_back(static_cast<char>(value));
}

inline void IOutputStream::Flush()
{
	std::ofstream out(path, std::ios::out | std::ios::binary);
	out.write(bytes.data(), bytes.size());
	out.close();
}
