#pragma once

#include <iostream>
#include <vector>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <string>

#if defined(_WIN32) and defined(_WIN64)
#include <Windows.h>
#endif

#define ref(A) A&
#define in :
#define foreach for
#define None void
#define List std::vector
#define Dictionary std::unordered_map
#define Tree std::map
#define Set std::set
#define MultiSet std::multiset
#define var auto
#define def auto
#define defn None
#define generic template
#define String std::string
#define Char char
#define Byte byte
#define Int16 int16_t
#define UInt16 uint16_t
#define Int32 int32_t
#define UInt32 uint32_t
#define Int64 int64_t
#define UInt64 uint64_t
#define Single float
#define Double double
#define Boolean bool
#define True true
#define False false
#define Exception std::exception
#define Greater std::greater
#define GreaterEqual std::greater_equal
#define Less std::less
#define LessEqual std::less_equal
#define Min *std::min_element
#define Max *std::max_element

struct ConsoleInputException : Exception
{
	const Char* what() const throw () override
	{
		return "Input error";
	}
};

defn print(const ref(String) end = "\n")
{
	std::cout << end;
}

generic<typename T>
defn print(const ref(T) value, const ref(String) end = "\n")
{
	std::cout << value << end;
}

generic<typename T>
Boolean input(ref(T) value, const ref(String) message = "")
{
	std::cout << message;
	
	std::cin >> value;
	if (std::cin.fail())
	{
		std::cin.clear();
		std::cin.ignore(32767, '\n');
		return False;
	}

	std::cin.ignore(32767, '\n');
	return True;
}

generic<typename T>
T input(const ref(String) message = "")
{
	T value;
	if (input(value, message))
	{
		return value;
	}

	throw ConsoleInputException();
}

generic<typename T>
Boolean input(ref(List<T>) values, const ref(String) message = "")
{
	std::cout << message;
	for (UInt64 i = 0; i < values.size(); i++)
	{
		if (std::cin >> values[i])
		{
			continue;
		}

		return False;
	}
	
	return True;
}

defn __main__(const ref(List<String>) args);

Int32 main(const Int32 argc, Char* argv[])
{
	#if defined(_WIN32) and defined(_WIN64)
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	#endif

	List<String> args(argc);

	for (Int32 i = 0; i < argc; i++)
	{
		args.emplace_back(String(argv[i]));
	}

	try
	{
		__main__(args);
		return 0;
	}
	catch(const ref(Exception) exception)
	{
		print(exception.what());
		return (Int32) &exception;
	}
}