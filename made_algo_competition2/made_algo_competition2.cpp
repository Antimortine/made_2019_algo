// Алгоритм сжатия данных:  BWT + MTF + Huffman


#include "Huffman.h"
#include <unordered_map>
#include <queue>
#include <iostream>
#include <algorithm>
#include <set>

// ========================================= BITWISE INPUT/OUTPUT =========================================

// Класс, реализующий операции побитовой и побайтовой записи. Результат возвращает в виде вектора байт.
// Последний байт хранит фактическое количество бит в предпоследнем байте. Ноль означает, что байт целиком хранит значимую информацию.
// Порядок записи бит в байте - от старших к младшим
class BitsWriter
{
public:
	void WriteBit(bool bit);
	void WriteByte(byte value);
	void WriteInt32(size_t value);

	std::vector<byte> GetResult();

private:
	std::vector<byte> buffer_;
	byte accumulator_ = 0;
	byte bits_count_ = 0;
};

// Класс, реализующий операции побитового и побайтового чтения из вектора байт.
// Предполагается, что последний байт хранит фактическое количество бит в предпоследнем байте (или 0, если байт целиком хранит значимую информацию)
// Порядок записи бит в байте - от старших к младшим
class BitsReader
{
public:
	explicit BitsReader(std::vector<byte>&& buffer);

	// Возвращает false, если поток закончился
	bool ReadBit(bool& bit);
	// Возвращает false, если поток закончился
	bool ReadByte(byte& value);
	// Возвращает false, если поток закончился
	bool ReadInt32(size_t& value);

private:
	std::vector<byte> buffer_;
	size_t current_byte = 0; // Индекс текущего байта
	byte bits_read = 0; // Сколько бит из текущего байта уже считано
	byte bits_in_last_byte;
};

void BitsWriter::WriteBit(bool bit)
{
	// Ставим бит в аккумулятор на нужное место
	accumulator_ |= static_cast<byte>(bit) << (7 - bits_count_++);
	if (bits_count_ == 8)
	{
		bits_count_ = 0;
		buffer_.push_back(accumulator_);
		accumulator_ = 0;
	}
}

void BitsWriter::WriteByte(byte value)
{
	if (bits_count_ == 0)
	{
		buffer_.push_back(value);
	}
	else
	{
		accumulator_ |= value >> bits_count_;
		buffer_.push_back(accumulator_);
		accumulator_ = value << (8 - bits_count_);
	}
}

void BitsWriter::WriteInt32(size_t value)
{
	WriteByte(static_cast<byte>((value >> 24) % 256));
	WriteByte(static_cast<byte>((value >> 16) % 256));
	WriteByte(static_cast<byte>((value >> 8) % 256));
	WriteByte(static_cast<byte>(value % 256));
}

std::vector<byte> BitsWriter::GetResult()
{
	if (bits_count_ != 0)
	{
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
	// current_byte указывает за границы буфера - считывать больше нечего
	if (current_byte == buffer_.size())
		return false;
	// current_byte указывает на последний байт, и уже считаны все его значимые биты
	if (current_byte == buffer_.size() - 1 && bits_read == bits_in_last_byte)
		return false;
	bit = static_cast<bool>(buffer_[current_byte] & (1 << (7 - bits_read++)));
	if (bits_read == 8)
	{
		bits_read = 0;
		current_byte++;
	}
	return true;
}

bool BitsReader::ReadByte(byte& value)
{
	// current_byte указывает за границы буфера - считывать больше нечего
	if (current_byte == buffer_.size())
		return false;
	// Если из текущего байта не считан ни один бит, можно считать его целиком
	if (bits_read == 0)
		value = buffer_[current_byte++];
	else
	{
		value = buffer_[current_byte++] << bits_read;
		if (current_byte != buffer_.size())
			value |= buffer_[current_byte] >> (8 - bits_read);
	}
	return true;
}

bool BitsReader::ReadInt32(size_t& value)
{
	byte byte0;
	byte byte1;
	byte byte2;
	byte byte3;

	if (!ReadByte(byte0) || !ReadByte(byte1) || !ReadByte(byte2) || !ReadByte(byte3))
		return false;

	value = byte3;
	value |= static_cast<size_t>(byte2) << 8;
	value |= static_cast<size_t>(byte1) << 16;
	value |= static_cast<size_t>(byte0) << 24;

	return true;
}

// ========================================= HUFFMAN CODING =========================================

class HuffmanCompressor
{
public:
	static void Encode(std::vector<byte>& original_bytes, std::vector<byte>& encoded_bytes, bool force_coding = false);
	static void Decode(std::vector<byte>& encoded_bytes, std::vector<byte>& original_bytes);

private:
	struct HuffmanTreeNode
	{
		explicit HuffmanTreeNode(const byte value, const long weight) : value(value), weight(weight)
		{
		}

		byte value;
		long weight;
		HuffmanTreeNode* left = nullptr;
		HuffmanTreeNode* right = nullptr;
	};

	static std::unordered_map<byte, long> GetFrequencies(const std::vector<byte>& bytes);
	static HuffmanTreeNode* BuildHuffmanTree(const std::unordered_map<byte, long>& frequencies);
	static std::unordered_map<byte, std::vector<bool>> BuildHuffmanCodes(HuffmanTreeNode* huffman_tree);
	static void BuildHuffmanCodes(HuffmanTreeNode* node, std::unordered_map<byte, std::vector<bool>>& codes_map,
		std::vector<bool>& code);
	static void DeleteTree(HuffmanTreeNode* node);

	static void EncodeTree(HuffmanTreeNode* node, BitsWriter& writer);
	static HuffmanTreeNode* DecodeTree(BitsReader& reader);

	static void EncodeMessage(std::vector<byte>& bytes, std::unordered_map<byte, std::vector<bool>>& huffman_codes,
		BitsWriter& writer);
	static void DecodeMessage(HuffmanTreeNode* huffman_tree, BitsReader& reader, BitsWriter& writer);
};

// Кодирует поток байтов original алгоритмом Хаффмана, записывает закодированные дерево Хаффмана и сообщение в поток compressed.
// Если force_coding==false и сжатое сообщение оказывается длиннее исходного, то записывает в выходной поток оригинальное сообщение
// со специальным сигнальным байтом в конце
void HuffmanCompressor::Encode(std::vector<byte>& original_bytes, std::vector<byte>& encoded_bytes, bool force_coding)
{
	std::unordered_map<byte, long> frequencies = GetFrequencies(original_bytes);
	HuffmanTreeNode* huffman_tree = BuildHuffmanTree(frequencies);
	std::unordered_map<byte, std::vector<bool>> huffman_codes = BuildHuffmanCodes(huffman_tree);

	BitsWriter writer;
	EncodeTree(huffman_tree, writer);
	DeleteTree(huffman_tree);
	EncodeMessage(original_bytes, huffman_codes, writer);

	encoded_bytes = writer.GetResult();

	// Если сжатый поток оказался не лучше оригинального, оставим его без изменений, добавив в конец особый байт
	if (!force_coding && original_bytes.size() + 1 <= encoded_bytes.size())
	{
		// BitsWriter в последнем байте хранит значение от 0 до 7 - количество значимых бит в предпоследнем байте
		// Если мы запишем туда значение >7, то при декодировании можно будет однозначно определить эту ситуацию
		original_bytes.push_back(static_cast<byte>(8));
		encoded_bytes = original_bytes;
	}
}

// Восстанавливает исходное сообщение, закодированное алгоритмом Хаффмана
void HuffmanCompressor::Decode(std::vector<byte>& encoded_bytes, std::vector<byte>& original_bytes)
{
	// Если последний байт больше 7, то остальные байты составляют исходное несжатое сообщение 
	byte last_byte = encoded_bytes.back();
	if (last_byte > 7)
	{
		encoded_bytes.pop_back();
		original_bytes = encoded_bytes;
		return;
	}

	BitsReader reader(std::move(encoded_bytes));

	HuffmanTreeNode* huffman_tree = DecodeTree(reader);
	BitsWriter writer;
	DecodeMessage(huffman_tree, reader, writer);
	DeleteTree(huffman_tree);

	original_bytes = writer.GetResult();
	original_bytes.pop_back();
}

// Строит таблицу частот байтов в данном векторе
std::unordered_map<byte, long> HuffmanCompressor::GetFrequencies(const std::vector<byte>& bytes)
{
	std::unordered_map<byte, long> frequencies;
	for (byte value : bytes)
	{
		if (frequencies.find(value) == frequencies.end())
			frequencies[value] = 1;
		else
			frequencies[value]++;
	}
	return frequencies;
}

// Строит таблицу кодов на основе дерева Хаффмана с использованием очереди с приоритетами
HuffmanCompressor::HuffmanTreeNode* HuffmanCompressor::BuildHuffmanTree(
	const std::unordered_map<byte, long>& frequencies)
{
	auto cmp = [](HuffmanTreeNode* left, HuffmanTreeNode* right) { return left->weight > right->weight; };
	std::priority_queue<HuffmanTreeNode*, std::vector<HuffmanTreeNode*>, decltype(cmp)> min_heap(cmp);
	for (auto& pair : frequencies)
		min_heap.push(new HuffmanTreeNode(pair.first, pair.second));
	while (min_heap.size() > 1)
	{
		HuffmanTreeNode* first = min_heap.top();
		min_heap.pop();
		HuffmanTreeNode* second = min_heap.top();
		min_heap.pop();
		auto new_node = new HuffmanTreeNode(0, first->weight + second->weight);
		new_node->left = first;
		new_node->right = second;
		min_heap.push(new_node);
	}
	HuffmanTreeNode* root = min_heap.top();
	min_heap.pop();
	return root;
}

// Строит таблицу кодов на основе дерева Хаффмана
std::unordered_map<byte, std::vector<bool>> HuffmanCompressor::BuildHuffmanCodes(HuffmanTreeNode* huffman_tree)
{
	std::unordered_map<byte, std::vector<bool>> codes_map;
	std::vector<bool> code;
	// Если дерево состоит всего из одного узла (в алфавите единственный символ), таблица строится тривиально
	if (!huffman_tree->left)
	{
		code.push_back(true);
		codes_map[huffman_tree->value] = code;
		return codes_map;
	}
	BuildHuffmanCodes(huffman_tree, codes_map, code);
	return codes_map;
}

// Строит код Хаффмана для каждого листового узла обходом в глубину
void HuffmanCompressor::BuildHuffmanCodes(HuffmanTreeNode* node, std::unordered_map<byte, std::vector<bool>>& codes_map,
	std::vector<bool>& code)
{
	if (!node->left)
	{
		codes_map[node->value] = std::vector<bool>(code);
		return;
	}
	code.push_back(false);
	BuildHuffmanCodes(node->left, codes_map, code);
	code.pop_back();

	code.push_back(true);
	BuildHuffmanCodes(node->right, codes_map, code);
	code.pop_back();
}

// Удаляет дерево обходом в глубину
void HuffmanCompressor::DeleteTree(HuffmanTreeNode* node)
{
	if (node->left)
		DeleteTree(node->left);
	if (node->right)
		DeleteTree(node->right);
	delete node;
}

// Кодирует структуру дерева Хаффмана прямым обходом в глубину
void HuffmanCompressor::EncodeTree(HuffmanTreeNode* node, BitsWriter& writer)
{
	// Листовой узел кодируется битом "1" и оригинальным байтом из словаря
	if (!node->left)
	{
		writer.WriteBit(true);
		writer.WriteByte(node->value);
		return;
	}
	// Все остальные узлы кодируются нулевым битом
	writer.WriteBit(false);
	EncodeTree(node->left, writer);
	EncodeTree(node->right, writer);
}

// Восстанавливает структуру дерева Хаффмана из кода, полученного с помощью EncodeTree
HuffmanCompressor::HuffmanTreeNode* HuffmanCompressor::DecodeTree(BitsReader& reader)
{
	bool is_leaf;
	reader.ReadBit(is_leaf);
	if (is_leaf)
	{
		byte value;
		reader.ReadByte(value);
		return new HuffmanTreeNode(value, 0);
	}
	HuffmanTreeNode* left = DecodeTree(reader);
	HuffmanTreeNode* right = DecodeTree(reader);
	auto* node = new HuffmanTreeNode(0, 0);
	node->left = left;
	node->right = right;
	return node;
}

void HuffmanCompressor::EncodeMessage(std::vector<byte>& bytes,
	std::unordered_map<byte, std::vector<bool>>& huffman_codes, BitsWriter& writer)
{
	for (byte value : bytes)
	{
		for (bool bit : huffman_codes[value])
			writer.WriteBit(bit);
	}
}

void HuffmanCompressor::DecodeMessage(HuffmanTreeNode* huffman_tree, BitsReader& reader, BitsWriter& writer)
{
	HuffmanTreeNode* node = huffman_tree;
	bool bit;
	while (reader.ReadBit(bit))
	{
		if (!bit)
			node = node->left;
		else
			node = node->right;
		if (!node) // Если словарь состоит всего из одного байта, в дереве будет единственный узел
			node = huffman_tree;
		if (!node->left)
		{
			writer.WriteByte(node->value);
			node = huffman_tree;
		}
	}
}

// ========================================= BWT =========================================

class BWTCodec
{
public:
	static void Encode(std::vector<byte>& original_bytes, std::vector<byte>& encoded_bytes);
	static void Decode(std::vector<byte>& encoded_bytes, std::vector<byte>& original_bytes);

private:
	struct ByteArrayRotation
	{
		explicit ByteArrayRotation(std::vector<byte>& array, size_t start) : array(array), start(start)
		{
		}

		std::vector<byte>& array;
		size_t start;

		byte operator [](size_t index) const
		{
			return array[(index + start) % array.size()];
		}

		bool operator< (const ByteArrayRotation& other) const
		{
			for (size_t i = 0; i < array.size(); ++i)
			{
				if ((*this)[i] != other[i])
					return (*this)[i] < other[i];
			}
			return false;
		}

		ByteArrayRotation& operator=(const ByteArrayRotation& other)
		{
			start = other.start;
			return *this;
		}
	};
};

void BWTCodec::Encode(std::vector<byte>& original_bytes, std::vector<byte>& encoded_bytes)
{
	std::vector<ByteArrayRotation> rotations;
	for (size_t i = 0; i < original_bytes.size(); ++i)
		rotations.push_back(ByteArrayRotation(original_bytes, i));
	std::sort(rotations.begin(), rotations.end());

	uint32_t original_position = 0;
	for (size_t i = 0; i < rotations.size(); ++i) {
		if (rotations[i].start == 0)
		{
			original_position = i;
			break;
		}
	}

	BitsWriter writer;
	writer.WriteInt32(original_position);
	for (size_t i = 0; i < rotations.size(); ++i)
	{
		auto rotation = rotations[i];
		byte last_value = rotation[rotation.array.size() - 1];
		writer.WriteByte(last_value);
	}
	encoded_bytes = writer.GetResult();
	encoded_bytes.pop_back(); // Число байт целое
}

void BWTCodec::Decode(std::vector<byte>& encoded_bytes, std::vector<byte>& original_bytes)
{
	encoded_bytes.push_back(0); // Для корректного считывания ридером нужно добавить метку того, что последний байт целый
	BitsReader reader(std::move(encoded_bytes));
	size_t original_position;
	reader.ReadInt32(original_position);

	std::vector<byte> bytes;
	byte value;
	while (reader.ReadByte(value))
		bytes.push_back(value);

	size_t byte_counts[256];
	for (int i = 0; i < 256; ++i)
		byte_counts[i] = 0;

	std::vector<size_t> byte_repetitions(bytes.size(), 0);

	for (size_t i = 0; i < bytes.size(); ++i)
		byte_repetitions[i] = byte_counts[bytes[i]]++;

	size_t total_sum = 0;
	for (int i = 0; i < 256; ++i)
	{
		size_t cnt = byte_counts[i];
		byte_counts[i] = total_sum;
		total_sum += cnt;
	}

	original_bytes.clear();
	original_bytes.assign(bytes.size(), 0);
	for (size_t i = bytes.size(); i > 0; --i)
	{
		original_bytes[i - 1] = bytes[original_position];
		original_position = byte_repetitions[original_position] + byte_counts[bytes[original_position]];
	}
}

// ========================================= RLE =========================================

class RLECodec
{
public:
	static void Encode(std::vector<byte>& original_bytes, std::vector<byte>& encoded_bytes);
	static void Decode(std::vector<byte>& encoded_bytes, std::vector<byte>& original_bytes);
};

void RLECodec::Encode(std::vector<byte>& original_bytes, std::vector<byte>& encoded_bytes)
{
	std::vector<byte> sequence;
	bool equal = true; // Набираем последовательность одинаковых байтов?
	for (byte value : original_bytes)
	{
		if (sequence.size() == 0)
		{
			sequence.push_back(value);
			equal = true;
			continue;
		}
		if (equal && sequence.back() == value)
		{
			if (sequence.size() == 127)
			{
				encoded_bytes.push_back(static_cast<byte>(127));
				encoded_bytes.push_back(value);
				sequence.clear();
			}
			sequence.push_back(value);
			continue;
		}
		if (!equal && sequence.back() != value)
		{
			if (sequence.size() == 128)
			{
				char len = static_cast<char>(-128);
				encoded_bytes.push_back(static_cast<byte>(len));
				encoded_bytes.insert(encoded_bytes.end(), sequence.begin(), sequence.end());
				sequence.clear();
			}
			sequence.push_back(value);
			continue;
		}
		if (equal && sequence.back() != value)
		{
			if (sequence.size() == 1)
			{
				sequence.push_back(value);
				equal = false;
				continue;
			}
			encoded_bytes.push_back(static_cast<byte>(sequence.size()));
			encoded_bytes.push_back(sequence.back());
			sequence.clear();
			sequence.push_back(value);
			continue;
		}
		if (!equal && sequence.back() == value)
		{
			equal = true;
			byte prev = sequence.back();
			sequence.pop_back();

			char len = -static_cast<char>(sequence.size());
			encoded_bytes.push_back(static_cast<byte>(len));
			encoded_bytes.insert(encoded_bytes.end(), sequence.begin(), sequence.end());
			sequence.clear();
			sequence.push_back(prev);
			sequence.push_back(value);
			continue;
		}
	}
	if (equal)
	{
		encoded_bytes.push_back(static_cast<byte>(sequence.size()));
		encoded_bytes.push_back(sequence.back());
		return;
	}
	char len = -static_cast<char>(sequence.size());
	encoded_bytes.push_back(static_cast<byte>(len));
	encoded_bytes.insert(encoded_bytes.end(), sequence.begin(), sequence.end());
}

void RLECodec::Decode(std::vector<byte>& encoded_bytes, std::vector<byte>& original_bytes)
{
	size_t i = 0;
	while (i < encoded_bytes.size())
	{
		char len = static_cast<char>(encoded_bytes[i]);
		if (len > 0)
		{
			byte repeated_value = encoded_bytes[i + 1];
			for (int j = 0; j < len; ++j)
				original_bytes.push_back(repeated_value);
			i += 2;
			continue;
		}
		size_t real_len = ~len + 1;
		for (int j = 1; j <= real_len; ++j)
			original_bytes.push_back(encoded_bytes[i + j]);
		i += real_len + 1;
	}
}

// ========================================= MTF =========================================

class MTFCodec
{
public:
	static void Encode(std::vector<byte>& original_bytes, std::vector<byte>& encoded_bytes);
	static void Decode(std::vector<byte>& encoded_bytes, std::vector<byte>& original_bytes);
};

void MTFCodec::Encode(std::vector<byte>& original_bytes, std::vector<byte>& encoded_bytes)
{
	std::vector<byte> dict;
	for (int val = 0; val < 256; ++val)
		dict.push_back(val);

	for (byte value : original_bytes)
	{
		byte index = std::distance(dict.begin(), std::find(dict.begin(), dict.end(), value));
		encoded_bytes.push_back(index);
		dict.erase(dict.begin() + index);
		dict.insert(dict.begin(), value);
	}
}

void MTFCodec::Decode(std::vector<byte>& encoded_bytes, std::vector<byte>& original_bytes)
{
	std::vector<byte> dict;
	for (int val = 0; val < 256; ++val)
		dict.push_back(val);

	for (byte index : encoded_bytes)
	{
		byte value = dict[index];
		original_bytes.push_back(value);
		dict.erase(dict.begin() + index);
		dict.insert(dict.begin(), value);
	}
}


void Encode(IInputStream& original, IOutputStream& compressed)
{
	std::vector<byte> original_bytes;
	byte value;
	while (original.Read(value))
	{
		original_bytes.push_back(value);
	}

	std::vector<byte> encoded_bytes;
	std::vector<byte> temp = std::vector<byte>(original_bytes);

	//RLECodec::Encode(temp, encoded_bytes);
	//temp = std::vector<byte>(encoded_bytes);
	//encoded_bytes.clear();

	BWTCodec::Encode(temp, encoded_bytes);
	temp = std::vector<byte>(encoded_bytes);
	encoded_bytes.clear();

	MTFCodec::Encode(temp, encoded_bytes);
	temp = std::vector<byte>(encoded_bytes);
	encoded_bytes.clear();

	//RLECodec::Encode(temp, encoded_bytes);
	//temp = std::vector<byte>(encoded_bytes);
	//encoded_bytes.clear();

	HuffmanCompressor::Encode(temp, encoded_bytes, true);
	if (original_bytes.size() + 1 < encoded_bytes.size())
	{
		encoded_bytes = original_bytes;
		encoded_bytes.push_back(static_cast<byte>(8));
	}

	for (byte val : encoded_bytes)
		compressed.Write(val);
}

void Decode(IInputStream& compressed, IOutputStream& original)
{
	std::vector<byte> compressed_bytes;
	byte value;
	while (compressed.Read(value))
	{
		compressed_bytes.push_back(value);
	}
	std::vector<byte> original_bytes;

	if (compressed_bytes.back() == 8)
	{
		compressed_bytes.pop_back();
		original_bytes = compressed_bytes;
	}
	else
	{
		std::vector<byte> temp = std::vector<byte>(compressed_bytes);
		HuffmanCompressor::Decode(temp, original_bytes);

		//temp = std::vector<byte>(original_bytes);
		//original_bytes.clear();
		//RLECodec::Decode(temp, original_bytes);

		temp = std::vector<byte>(original_bytes);
		original_bytes.clear();
		MTFCodec::Decode(temp, original_bytes);

		temp = std::vector<byte>(original_bytes);
		original_bytes.clear();
		BWTCodec::Decode(temp, original_bytes);

		//temp = std::vector<byte>(original_bytes);
		//original_bytes.clear();
		//RLECodec::Decode(temp, original_bytes);
	}

	for (byte val : original_bytes)
		original.Write(val);
}

//void test(const std::string& extension)
//{
//	auto input = FInputStream("data/input." + extension);
//	auto compressed_output = FOutputStream("data/compressed." + extension);
//	Encode(input, compressed_output);
//	compressed_output.Flush();
//
//	auto compressed_input = FInputStream("data/compressed." + extension);
//	auto decompressed_output = FOutputStream("data/decompressed." + extension);
//	Decode(compressed_input, decompressed_output);
//	decompressed_output.Flush();
//}
//
//int main()
//{
//	test("txt");
//	test("bmp");
//	test("jpg");
//
//	return 0;
//}
