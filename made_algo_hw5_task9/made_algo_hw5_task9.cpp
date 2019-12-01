// Задача 9. Алгоритм сжатия данных Хаффмана
// (15 баллов)
// Реализуйте алгоритм построения оптимального префиксного кода Хаффмана.
// При помощи алгоритма реализуйте две функции для создания архива из одного файла и извлечения файла из архива.


#include "Huffman.h"
#include <unordered_map>
#include <queue>

// ========================================= BITWISE INPUT/OUTPUT =========================================

// Порядок записи бит в байте - от старших к младшим
class BitsWriter
{
public:
	void WriteBit(bool bit);
	void WriteByte(byte value);

	std::vector<byte> GetResult();

private:
	std::vector<byte> buffer_;
	byte accumulator_ = 0;
	byte bits_count_ = 0;
};

// Порядок записи бит в байте - от старших к младшим
class BitsReader
{
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
	if (current_byte == buffer_.size())
		return false;
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
	if (current_byte == buffer_.size())
		return false;
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

// ========================================= HUFFMAN CODING =========================================

class HuffmanCompressor
{
public:
	static void Encode(IInputStream& original, IOutputStream& compressed);
	static void Decode(IInputStream& compressed, IOutputStream& original);

private:
	struct HuffmanTreeNode
	{
		explicit HuffmanTreeNode(const byte value, const long weight): value(value), weight(weight)
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

void HuffmanCompressor::Encode(IInputStream& original, IOutputStream& compressed)
{
	std::vector<byte> original_bytes;
	byte value;
	while (original.Read(value))
	{
		original_bytes.push_back(value);
	}

	std::unordered_map<byte, long> frequencies = GetFrequencies(original_bytes);
	HuffmanTreeNode* huffman_tree = BuildHuffmanTree(frequencies);
	std::unordered_map<byte, std::vector<bool>> huffman_codes = BuildHuffmanCodes(huffman_tree);

	BitsWriter writer;
	EncodeTree(huffman_tree, writer);
	DeleteTree(huffman_tree);
	EncodeMessage(original_bytes, huffman_codes, writer);

	std::vector<byte> compressed_bytes = writer.GetResult();
	for (byte val : compressed_bytes)
	{
		compressed.Write(val);
	}
}

void HuffmanCompressor::Decode(IInputStream& compressed, IOutputStream& original)
{
	std::vector<byte> compressed_bytes;
	byte value;
	while (compressed.Read(value))
	{
		compressed_bytes.push_back(value);
	}
	BitsReader reader(std::move(compressed_bytes));

	HuffmanTreeNode* huffman_tree = DecodeTree(reader);
	BitsWriter writer;
	DecodeMessage(huffman_tree, reader, writer);
	DeleteTree(huffman_tree);

	std::vector<byte> original_bytes = writer.GetResult();
	original_bytes.pop_back();
	for (byte val : original_bytes)
	{
		original.Write(val);
	}
}

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

std::unordered_map<byte, std::vector<bool>> HuffmanCompressor::BuildHuffmanCodes(HuffmanTreeNode* huffman_tree)
{
	std::unordered_map<byte, std::vector<bool>> codes_map;
	std::vector<bool> code;
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

void HuffmanCompressor::DeleteTree(HuffmanTreeNode* node)
{
	if (node->left)
		DeleteTree(node->left);
	if (node->right)
		DeleteTree(node->right);
	delete node;
}

void HuffmanCompressor::EncodeTree(HuffmanTreeNode* node, BitsWriter& writer)
{
	if (!node->left)
	{
		writer.WriteBit(true);
		writer.WriteByte(node->value);
		return;
	}
	writer.WriteBit(false);
	EncodeTree(node->left, writer);
	EncodeTree(node->right, writer);
}

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
	while(reader.ReadBit(bit))
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

void Encode(IInputStream& original, IOutputStream& compressed)
{
	HuffmanCompressor::Encode(original, compressed);
}

void Decode(IInputStream& compressed, IOutputStream& original)
{
	HuffmanCompressor::Decode(compressed, original);
}

// int main()
// {
// 	auto input = IInputStream("test_input.txt");
// 	auto compressed_output = IOutputStream("compressed.txt");
// 	Encode(input, compressed_output);
// 	compressed_output.Flush();
//
// 	auto compressed_input = IInputStream("compressed.txt");
// 	auto decompressed_output = IOutputStream("decompressed.txt");
// 	Decode(compressed_input, decompressed_output);
// 	decompressed_output.Flush();
//
// 	return 0;
// }
