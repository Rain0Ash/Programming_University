//В некоторых проектах используется python++, в данном будет использоваться PSharp++
#include "PSharp++.h"

defn __main__(const ref(List<String>) args)
{
	MultiSet<Int32, Greater<Int32>> set;

	const Int32 n = input<Int32>("Введите число n: ");
	List<Int32> values(n);

	if (!input(values, "Введите n чисел, разделённые пробелом: "))
	{
		print("Ошибка ввода!");
		return;
	}

	const UInt64 items = 5;

	foreach(var value in values)
	{
		if (set.size() >= items and value < Max(set.begin(), set.end()))
		{
			set.erase(Max(set.begin(), set.end()));
			set.insert(value);
		}
		else if (set.size() < items)
		{
			set.insert(value);
		}

		std::ostringstream stream;
		
		for (const ref(var) value in set)
		{
			stream << value << " ";
		}

		String str = stream.str();
		print(str.substr(0, str.size() - 1), "\n");

		stream.clear();
	}
}