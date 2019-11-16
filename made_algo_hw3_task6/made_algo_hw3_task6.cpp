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

	void print() const;

	void add(int value);

private:
	BinaryTreeNode* root = nullptr;

	static void delete_subtree(BinaryTreeNode* node);
};

BinaryTree::~BinaryTree()
{
	delete_subtree(root);
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

// Печатает элементы дерева в порядке level-order
void BinaryTree::print() const
{
	if (!root)
		return;
	std::queue<BinaryTreeNode*> nodes;
	nodes.push(root);
	while (!nodes.empty())
	{
		BinaryTreeNode* node = nodes.front();
		nodes.pop();
		std::cout << node->value << " ";
		if (node->left)
			nodes.push(node->left);
		if (node->right)
			nodes.push(node->right);
	}
}

// Удаляет элементы поддерева в порядке level-order
void BinaryTree::delete_subtree(BinaryTreeNode* node)
{
	if (!node)
		return;
	std::queue<BinaryTreeNode*> nodes;
	nodes.push(node);
	while (!nodes.empty())
	{
		node = nodes.front();
		nodes.pop();
		if (node->left)
			nodes.push(node->left);
		if (node->right)
			nodes.push(node->right);
		delete node;
	}
}

int main()
{
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);

	int n = 0;
	std::cin >> n;

	BinaryTree tree;

	for (int i = 0; i < n; ++i)
	{
		int value = 0;
		std::cin >> value;
		tree.add(value);
	}

	tree.print();

	std::cout.flush();
	return 0;
}
