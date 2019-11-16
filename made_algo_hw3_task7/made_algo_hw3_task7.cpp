// Солдаты. В одной военной части решили построить в одну шеренгу по росту.
// Т.к. часть была далеко не образцовая, то солдаты часто приходили не вовремя,
// а то их и вовсе приходилось выгонять из шеренги за плохо начищенные сапоги.
// Однако солдаты в процессе прихода и ухода должны были всегда быть выстроены по росту – сначала самые высокие,
// а в конце – самые низкие. За расстановку солдат отвечал прапорщик, который заметил интересную особенность –
// все солдаты в части разного роста. Ваша задача состоит в том, чтобы помочь прапорщику правильно расставлять солдат,
// а именно для каждого приходящего солдата указывать, перед каким солдатом в строе он должен становиться.
// Вариант 7_1. Требуемая скорость выполнения команды - O(log n) в среднем. В реализации используйте декартово дерево.
//
#include <iostream>
#include <random>

// Узел декартова дерева.
struct TreapNode
{
	explicit TreapNode(int _value) :
		value(_value), priority(rand())
	{
	}

	int value;
	int priority;
	int subtree_size = 1; // Размер поддерева с корнем в данном узле
	TreapNode* left = nullptr;
	TreapNode* right = nullptr;
};

// Декартово дерево
class Treap
{
public:
	~Treap();

	int add(int value);

	void remove(int value);

	int get_k_order_statistic(int k) const;

	int size() const;

private:
	TreapNode* root = nullptr;

	static void delete_subtree(TreapNode* node);

	static std::pair<TreapNode*, TreapNode*> split(TreapNode* node, int value);
	
	static TreapNode* merge(TreapNode* left, TreapNode* right);

	static int get_size(TreapNode* node);

	static int get_k_order_statistic(TreapNode* node, int k);
	
};

// Разделяет поддерево node на два: в первом значения не превосходят value, во втором строго больше
std::pair<TreapNode*, TreapNode*> Treap::split(TreapNode* node, int value)
{
	if (!node)
	{
		return std::make_pair(static_cast<TreapNode*>(nullptr), static_cast<TreapNode*>(nullptr));
	}
	if (node->value <= value) // Левое поддерево не изменится
	{
		const auto right_pair = split(node->right, value);
		node->right = right_pair.first;
		node->subtree_size = get_size(node);
		return std::make_pair(node, right_pair.second);
	}
	else // Правое поддерево не изменится
	{
		const auto left_pair = split(node->left, value);		
		node->left = left_pair.second;
		node->subtree_size = get_size(node);
		return std::make_pair(left_pair.first, node);
	}
}

// Сливает два дерева left и right в одно
// предполагается, что все ключи в left не превышают ключей в right
TreapNode* Treap::merge(TreapNode* left, TreapNode* right)
{
	if (!left) return right;
	if (!right) return left;

	if (right->priority < left->priority) // Новым корнем станет left, т.к. у него наивысший приоритет
	{
		left->right = merge(left->right, right);
		left->subtree_size = get_size(left);
		return left;
	}
	else
	{
		right->left = merge(left, right->left);
		right->subtree_size = get_size(right);
		return right;
	}
}

Treap::~Treap()
{
	delete_subtree(root);
}

void Treap::delete_subtree(TreapNode* node)
{
	if (!node) return;
	delete_subtree(node->left);
	delete_subtree(node->right);
	delete node;
}

// Добавляет элемент value в дерево и возвращает количество элементов, не превышающих value
int Treap::add(const int value)
{
	const auto new_node = new TreapNode(value);
	const auto pair = split(root, value);
	const auto greater_elements_count = get_size(pair.second);
	const auto new_left = merge(pair.first, new_node);
	new_left->subtree_size = get_size(new_left);
	root = merge(new_left, pair.second);
	root->subtree_size = get_size(root);
	return greater_elements_count;
}

void Treap::remove(const int value)
{
	const auto pair = split(root, value - 1);
	const auto new_left = pair.first;
	const auto right_split = split(pair.second, value);
	root = merge(new_left, right_split.second);
	root->subtree_size = get_size(root);
}

// Возвращает количество узлов в поддереве node
int Treap::get_size(TreapNode* node)
{
	if (!node) return 0;
	int size = 1;
	if (node->left)
		size += node->left->subtree_size;
	if (node->right)
		size += node->right->subtree_size;
	return size;
}

// Возвращает значение элемента, стоящего в позиции k при сортировке по возрастанию
int Treap::get_k_order_statistic(const int k) const
{
	return get_k_order_statistic(root, k);
}

int Treap::get_k_order_statistic(TreapNode* node, const int k)
{
	if (get_size(node->left) == k)
		return node->value;
	if (get_size(node->left) > k)
		return get_k_order_statistic(node->left, k);
	return get_k_order_statistic(node->right, k - get_size(node->left) - 1);
}

// Количество элементов в дереве
int Treap::size() const
{
	return root->subtree_size;
}

int main()
{
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);
	srand(42);

	int n = 0;
	std::cin >> n;

	Treap treap;

	for (int i = 0; i < n; ++i)
	{
		int command = 0;
		int value = 0;
		std::cin >> command >> value;
		if (command == 1) // солдат ростом value приходит в строй
		{
			int position = treap.add(value);
			std::cout << position << " ";
		}
		else // солдата на месте value надо удалить из строя
		{
			int height = treap.get_k_order_statistic(treap.size() - 1 - value); // Рост солдата в позиции value
			treap.remove(height);
		}
	}

	std::cout.flush();
	return 0;
}
