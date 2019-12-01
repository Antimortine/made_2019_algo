// ������ 9. �������� ������ ������ ��������
// (15 ������)
// ���������� �������� ���������� ������������ ����������� ���� ��������.
// ��� ������ ��������� ���������� ��� ������� ��� �������� ������ �� ������ ����� � ���������� ����� �� ������.


#include "Huffman.h"

static void CopyStream(IInputStream& input, IOutputStream& output)
{
	byte value;
	while (input.Read(value))
	{
		output.Write(value);
	}
}

void Encode(IInputStream& original, IOutputStream& compressed)
{
	CopyStream(original, compressed);
}

void Decode(IInputStream& compressed, IOutputStream& original)
{
	CopyStream(compressed, original);
}


int main()
{
	auto input = IInputStream("test_input.txt");
	auto output = IOutputStream("test_output.txt");
	CopyStream(input, output);
	output.Flush();

	input = IInputStream("test_input.bmp");
	output = IOutputStream("test_output.bmp");
	CopyStream(input, output);
	output.Flush();

	return 0;
}
