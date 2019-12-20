#include "DisjointSetUnion.hpp"
#include <cassert>

DisjointSetUnion::DisjointSetUnion(size_t size) :
	parent(size),
	rank(size, 0)
{
	for (size_t i = 0; i < parent.size(); ++i)
		parent[i] = i;
}

size_t DisjointSetUnion::find_representative(size_t element)
{
	if (parent[element] == element)
		return element;
	return parent[element] = find_representative(parent[element]);
}

void DisjointSetUnion::merge(size_t left_representative, size_t right_representative)
{
	assert(left_representative != right_representative);
	if (rank[left_representative] == rank[right_representative])
	{
		parent[right_representative] = left_representative;
		++rank[left_representative];
	}
	else if (rank[left_representative] > rank[right_representative])
	{
		parent[right_representative] = left_representative;
	}
	else
	{
		parent[left_representative] = right_representative;
	}
}