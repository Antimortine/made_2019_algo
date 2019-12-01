// Задача 9. Алгоритм сжатия данных Хаффмана
// (15 баллов)
// Реализуйте алгоритм построения оптимального префиксного кода Хаффмана.
// При помощи алгоритма реализуйте две функции для создания архива из одного файла и извлечения файла из архива.


#include "Huffman.h"

// Порядок записи бит в байте - от младших к старшим
class BitsWriter {
public:
	void WriteBit(bool bit);
	void WriteByte(byte value);

	std::vector<byte> GetResult();

private:
	std::vector<byte> buffer_;
	byte accumulator_ = 0;
	byte bits_count_ = 0;
};

// Порядок записи бит в байте - от младших к старшим
class BitsReader {
public:
	explicit BitsReader(std::vector<byte>&& buffer);
	
	// Возвращают false, если поток закончился
	bool ReadBit(bool& bit);
	bool ReadByte(byte& value);

private:
	std::vector<byte> buffer_;
	size_t current_byte = 0;
	byte bits_read = 0; // Сколько бит из текущего байта уже считано
	byte bits_in_last_byte;
};

void BitsWriter::WriteBit(bool bit) {
	// Ставим бит в аккумулятор на нужное место
	accumulator_ |= static_cast<byte>(bit) << bits_count_++;
	if (bits_count_ == 8) {
		bits_count_ = 0;
		buffer_.push_back(accumulator_);
		accumulator_ = 0;
	}
}

void BitsWriter::WriteByte(byte value) {
	if (bits_count_ == 0) {
		buffer_.push_back(value);
	}
	else {
		accumulator_ |= value << bits_count_;
		buffer_.push_back(accumulator_);
		accumulator_ = value >> (8 - bits_count_);
	}
}

std::vector<byte> BitsWriter::GetResult() {
	if (bits_count_ != 0) {
		// Добавляем в буфер аккумулятор, если в нем что-то есть.
		buffer_.push_back(accumulator_);
	}
	buffer_.push_back(bits_count_);
	return std::move(buffer_);
}

BitsReader::BitsReader(std::vector<byte>&& buffer) : buffer_(std::move(buffer))
{
	bits_in_last_byte = buffer_.back();
	if (bits_in_last_byte == 0)
		bits_in_last_byte = 8;
	buffer_.pop_back();
}

bool BitsReader::ReadBit(bool& bit)
{
	if (current_byte == buffer_.size())
		return false;
	if (current_byte == buffer_.size() - 1 && bits_read == bits_in_last_byte)
		return false;
	bit = static_cast<bool>(buffer_[current_byte] & (1 << bits_read++));
	if (bits_read == 8)
	{
		bits_read = 0;
		current_byte++;
	}
	return true;
}

bool BitsReader::ReadByte(byte& value)
{
	if (current_byte == buffer_.size())
		return false;
	if (bits_read == 0)
		value = buffer_[current_byte++];
	else
	{
		value = buffer_[current_byte++] >> bits_read;
		if (current_byte != buffer_.size())
			value |= buffer_[current_byte] << (8 - bits_read);
	}
	return true;
}

void Encode(IInputStream& original, IOutputStream& compressed)
{
	BitsWriter bits_writer;
	byte value;
	while (original.Read(value))
	{
		bits_writer.WriteByte(value);
	}
	std::vector<byte> bytes = bits_writer.GetResult();
	for(byte val : bytes)
	{
		compressed.Write(val);
	}
}

void Decode(IInputStream& compressed, IOutputStream& original)
{
	std::vector<byte> bytes;
	byte value;
	while (compressed.Read(value))
	{
		bytes.push_back(value);
	}
	BitsReader bits_reader(std::move(bytes));
	while (bits_reader.ReadByte(value))
	{
		original.Write(value);
	}
}

int main()
{
	auto input = IInputStream("test_input.txt");
	auto compressed_output = IOutputStream("compressed.txt");
	Encode(input, compressed_output);
	compressed_output.Flush();

	auto compressed_input = IInputStream("compressed.txt");
	auto decompressed_output = IOutputStream("decompressed.txt");
	Decode(compressed_input, decompressed_output);
	decompressed_output.Flush();
	
	// input = IInputStream("test_input.bmp");
	// output = IOutputStream("test_output.bmp");
	// CopyStream(input, output);
	// output.Flush();

	return 0;
}
