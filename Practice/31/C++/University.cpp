﻿#include <vector>
#include <fstream>

std::ostream& operator<<(std::ostream& stream, std::vector<int>& vector)
{
	stream << vector.size() << "\t|";

	for (auto& elem : vector)
	{
		stream << ' ' << &elem;
	}
	
	return stream;
}

int main()
{
	std::vector<int> vector;
	std::ofstream file("data.txt");

	// 1. Адреса элементов являются различными, так как при вызове оператора << производится копирования вектора, а адреса последующих элементов инкрементируются на sizeof(T), так как элементы распологаются последовательно.

	// 2. Первый элемент может быть как изменен, так и нет. Это зависит от того, воможно ли добавить дополнительные элементы без перемещения вектора из-за нехватки аллоцированой памяти.

	for (int i = 0; i < 64; ++i)
	{
		vector.push_back(0);
		file << vector << std::endl;
	}
	
	file << std::endl;

	// 3. Соответствует ответу на вопрос 1.
	while (!vector.empty())
	{
		vector.pop_back();
		file << vector << std::endl;
	}

	file.close();

	/* 4.
	   Если общий размер элементов с учетом добавления новых превышает аллоцированный размер памяти, то будет аллоцирована новая память достаточного размера и копирует элементы. После этого старая память удаляется.

	   При использовании метода pop_back размер вектора уменьшается, однако переаллокации памяти не будет. Поэтому адрес первого элемента останется тем же.
	*/
}