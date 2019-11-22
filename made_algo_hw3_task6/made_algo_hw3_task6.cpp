//  Дано число N < 10^6 и последовательность целых чисел из[-2^31..2^31] длиной N.
// 	Требуется построить бинарное дерево, заданное наивным порядком вставки.
// 	Т.е., при добавлении очередного числа K в дерево с корнем root, если root→Key ≤ K, то узел K добавляется в правое поддерево root; иначе в левое поддерево root.
// 	Рекурсия запрещена.
//  6_4. Выведите элементы в порядке level-order (по слоям, “в ширину”).
//

#include <iostream>
#include <queue>

// Узел бинарного дерева
struct BinaryTreeNode
{
	explicit BinaryTreeNode(int _value) : value(_value)
	{
	}

	int value;
	BinaryTreeNode* left = nullptr;
	BinaryTreeNode* right = nullptr;
};

// Бинарное дерево поиска
class BinaryTree
{
public:
	~BinaryTree();

	void traverse_level_order(void (*action)(BinaryTreeNode*));

	void add(int value);

private:
	BinaryTreeNode* root = nullptr;
};

BinaryTree::~BinaryTree()
{
	traverse_level_order([](BinaryTreeNode* node)
	{
		delete node;
	});
}

// Добавляет новый элемент в бинарное дерево поиска без использования рекурсии
void BinaryTree::add(int value)
{
	if (!root)
	{
		root = new BinaryTreeNode(value);
	}
	else
	{
		BinaryTreeNode* node = root;
		while (true)
		{
			if (node->value <= value)
			{
				if (!node->right)
				{
					node->right = new BinaryTreeNode(value);
					return;
				}
				node = node->right;
			}
			else
			{
				if (!node->left)
				{
					node->left = new BinaryTreeNode(value);
					return;
				}
				node = node->left;
			}
		}
	}
}

// Обходит элементы дерева и применяет к ним функцию action в порядке level-order
void BinaryTree::traverse_level_order(void (*action)(BinaryTreeNode*))
{
	if (!root)
		return;
	std::queue<BinaryTreeNode*> nodes;
	nodes.push(root);
	while (!nodes.empty())
	{
		auto node = nodes.front();
		nodes.pop();
		BinaryTreeNode* left = node->left;
		BinaryTreeNode* right = node->right;
		action(node);
		if (left)
			nodes.push(left);
		if (right)
			nodes.push(right);
	}
}

int main()
{
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);

	int n = 0;
	std::cin >> n;

	BinaryTree tree;

	auto print_node_value = [](BinaryTreeNode* node)
	{
		std::cout << node->value << " ";
	};

	for (int i = 0; i < n; ++i)
	{
		int value = 0;
		std::cin >> value;
		tree.add(value);
	}

	tree.traverse_level_order(print_node_value);

	std::cout.flush();
	return 0;
}
