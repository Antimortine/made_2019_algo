// 1_3. Реализовать очередь с помощью двух стеков. Использовать стек, реализованный с помощью динамического буфера.
//

#include <assert.h>
#include <iostream>

class Stack
{
public:
	~Stack();

	// Проверка стека на пустоту
	bool empty() const;
	// Добавление элемента
	void push(int value);
	// Извлечение
	int pop();
	// Количество элементов в стеке
	int size() const;

private:
	int capacity = 32; // Размер буфера
	int count = 0; // Количество элементов в стеке
	int* buffer = new int[capacity];
};

class Queue
{
public:
	~Queue();

	// Проверка очереди на пустоту
	bool empty() const;
	// Добавление элемента
	void push(int value);
	// Извлечение
	int pop();
	// Количество элементов в очереди
	int size() const;

private:
	Stack* forward_stack = new Stack(); // Стек для операции push
	Stack* backward_stack = new Stack(); // Стек для операции pop
};

Stack::~Stack()
{
	delete[] buffer;
}

bool Stack::empty() const
{
	return count == 0;
}

void Stack::push(int value)
{
	if (count == capacity)
	{
		int* new_buffer = new int[capacity * 2];
		for (int i = 0; i < capacity; ++i)
			new_buffer[i] = buffer[i];
		delete[] buffer;
		buffer = new_buffer;
		capacity *= 2;
	}
	buffer[count++] = value;
}

int Stack::pop()
{
	assert(!empty());
	return buffer[count-- - 1];
}

int Stack::size() const
{
	return count;
}

Queue::~Queue()
{
	delete forward_stack;
	delete backward_stack;
}

bool Queue::empty() const
{
	return forward_stack->empty() && backward_stack->empty();
}

void Queue::push(int value)
{
	forward_stack->push(value);
}

int Queue::pop()
{
	assert(!empty());
	if (backward_stack->empty())
		while (!forward_stack->empty())
			backward_stack->push(forward_stack->pop());
	return backward_stack->pop();
}

int Queue::size() const
{
	return forward_stack->size() + backward_stack->size();
}

int main()
{
	int n = 0;
	std::cin >> n;

	Queue queue;
	for (int i = 0; i < n; ++i)
	{
		int command = 0;
		int value = 0;
		std::cin >> command >> value;
		switch (command)
		{
		case 3:
			queue.push(value);
			break;
		case 2:
			if (queue.empty())
			{
				if (value != -1)
				{
					std::cout << "NO";
					return 0;
				}
			}
			else if (queue.pop() != value)
			{
				std::cout << "NO";
				return 0;
			}
			break;
		default:
			return -1;
		}
	}
	std::cout << "YES";
	return 0;
}
