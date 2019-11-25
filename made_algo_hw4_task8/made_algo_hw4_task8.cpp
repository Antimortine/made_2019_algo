// ������ 8. ���-������� (10 ������)
// ���������� ��������� ������ ���� ���������� ����� �� ������ ������������ ��� - ������� � �������� ����������.
// �������� ������ �������� � ������� �� �������� ��������� ����.
// ��� - ������� ������ ������ ���� ����������� � ������� ���������� �������� ���������� ������� �������.
// ��������� ������ ������� ������ ���� ������ 8 - ��. ��������������� ���������� ��� ���������� ��������� � ������, ����� ����������� ���������� ������� ��������� 3 / 4.
// ��������� ������ ������ ������������ �������� ���������� ������ � ���������, �������� ������ �� ��������� � �������� �������������� ������ ������ ���������.
// 1_1. ��� ���������� �������� ����������� ������������ ������������.
// i - �� ����� g(k, i) = g(k, i - 1) + i(mod m).m - ������� ������.
//

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

class HashTable
{
public:
	explicit HashTable(size_t initial_size = 8);
	~HashTable();
	HashTable(const HashTable&) = delete;
	HashTable(HashTable&&) = delete;
	HashTable& operator=(const HashTable&) = delete;
	HashTable& operator=(HashTable&&) = delete;

	bool has(const std::string& key) const;
	bool add(const std::string& key);
	bool remove(const std::string& key);

private:
	struct HashTableEntry
	{
		std::string key;
		bool deleted = false;
		size_t hash;

		explicit HashTableEntry(std::string key_, size_t hash_) : key(std::move(key_)), hash(hash_)
		{
		}
	};

	std::vector<HashTableEntry*> table;
	const double rehash_threshold = 3.0 / 4;
	size_t cells_used = 0; // ���������� �����, ���� ���������� �������, ���� ���������� ��� deleted
	double get_load_factor() const; // ����������� ����������
	static size_t get_hash(const std::string& key);
	size_t probe(const size_t hash, const size_t i) const;
	void rehash();
	bool add(const std::string& key, const size_t hash);
};

HashTable::HashTable(size_t initial_size) : table(initial_size, nullptr)
{
	assert(initial_size > 0 && (initial_size & initial_size - 1) == 0); // initial_size ������ ���� �������� ������
}

HashTable::~HashTable()
{
	for (HashTableEntry* entry : table)
		delete entry;
}

bool HashTable::has(const std::string& key) const
{
	const size_t hash = get_hash(key);
	const size_t M = table.size();
	for (size_t i = 0; i < M; ++i)
	{
		const size_t index = probe(hash, i);
		if (!table[index])
			return false;
		if (table[index]->deleted)
			continue;
		if (table[index]->hash == hash && table[index]->key == key)
			// ��������� ����� ��������� �������� ������������ ������� ��������� �����
			return true;
	}
	return false;
}

// ������� � ��������� ����� ������������ ��� ���������������
bool HashTable::add(const std::string& key, const size_t hash)
{
	const size_t M = table.size();
	for (size_t i = 0; i < M; ++i)
	{
		const size_t index = probe(hash, i);
		if (!table[index])
		{
			table[index] = new HashTableEntry(key, hash);
			cells_used += 1;
			if (get_load_factor() >= rehash_threshold)
				rehash();
			return true;
		}
		if (table[index]->deleted)
			continue;
		if (table[index]->hash == hash && table[index]->key == key)
			// ��������� ����� ��������� �������� ������������ ������� ��������� �����
			return false;
	}
	return false;
}

bool HashTable::add(const std::string& key)
{
	const size_t hash = get_hash(key);
	return add(key, hash);
}

bool HashTable::remove(const std::string& key)
{
	const size_t hash = get_hash(key);
	const size_t M = table.size();
	for (size_t i = 0; i < M; ++i)
	{
		const size_t index = probe(hash, i);
		if (!table[index])
			return false;
		if (table[index]->deleted)
			continue;
		if (table[index]->hash == hash && table[index]->key == key)
			// ��������� ����� ��������� �������� ������������ ������� ��������� �����
		{
			table[index]->deleted = true;
			return true;
		}
	}
	return false;
}

double HashTable::get_load_factor() const
{
	return static_cast<double>(cells_used) / table.size();
}

// �������������� ��� �� ������ �������
size_t HashTable::get_hash(const std::string& key)
{
	const size_t a = 22695477;
	size_t hash = 0;
	for (const char& c : key)
	{
		hash = hash * a + c; // ������� �� ������� �� ������ ������� ����� ����� ��������
	}
	return hash;
}

// ������������ ������������
size_t HashTable::probe(const size_t hash, const size_t i) const
{
	return (hash + i * (i + 1) / 2) % table.size();
}

// �������� ������� �� ����� ������� ������������ � ������� ���� ���� ���������� ���������
void HashTable::rehash()
{
	const size_t M = table.size();
	std::vector<HashTableEntry*> current_table = table;
	table = std::vector<HashTableEntry*>(M * 2, nullptr);
	for (HashTableEntry* entry : current_table)
	{
		if (!entry)
			continue;
		if (entry->deleted)
			delete entry;
		else
		{
			add(entry->key, entry->hash);
			delete entry;
		}
	}
}

int main()
{
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);

	HashTable table;
	char command = ' ';
	std::string value;
	while (std::cin >> command >> value)
	{
		switch (command)
		{
		case '?':
			std::cout << (table.has(value) ? "OK" : "FAIL") << "\n";
			break;
		case '+':
			std::cout << (table.add(value) ? "OK" : "FAIL") << "\n";
			break;
		case '-':
			std::cout << (table.remove(value) ? "OK" : "FAIL") << "\n";
			break;
		}
	}

	std::cout.flush();
	return 0;
}
