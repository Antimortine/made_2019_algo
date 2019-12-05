#pragma once

#include <string>
#include <fstream>
#include <iterator>
#include <vector>

typedef unsigned char byte;

// Входной поток
struct IInputStream
{
	// Возвращает false, если поток закончился
	virtual bool Read(byte& value) = 0;
};

// Выходной поток
struct IOutputStream
{
	virtual void Write(byte value) = 0;
};

class FInputStream : public IInputStream
{
public:
	explicit FInputStream(const std::string& path);
	bool Read(byte& value) override;

private:
	std::vector<char> bytes;
	size_t position = 0;
};

class FOutputStream : public IOutputStream
{
public:
	explicit FOutputStream(std::string path) : path(std::move(path))
	{
	}

	void Write(byte value) override;
	void Flush();

private:
	std::vector<char> bytes;
	std::string path;
};

inline FInputStream::FInputStream(const std::string& path)
{
	std::ifstream input(path, std::ios::binary);
	bytes = std::vector<char>(
		std::istreambuf_iterator<char>(input),
		std::istreambuf_iterator<char>());
	input.close();
}

inline bool FInputStream::Read(byte& value)
{
	if (position == bytes.size())
		return false;
	value = static_cast<byte>(bytes[position++]);
	return true;
}

inline void FOutputStream::Write(byte value)
{
	bytes.push_back(static_cast<char>(value));
}

inline void FOutputStream::Flush()
{
	std::ofstream out(path, std::ios::out | std::ios::binary);
	out.write(bytes.data(), bytes.size());
	out.close();
}
