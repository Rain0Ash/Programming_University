#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>

#define nameof(name) #name

template <class T>
using vector_t = std::vector<T>;

template <class T>
bool is_more(const T& first, const T& second)
{
	return first > second;
}

template <class T>
bool is_less(const T& first, const T& second)
{
	return first < second;
}

template <class T>
bool is_sorted(const vector_t<T>& vector, const bool& asc)
{
	const auto sort = asc ? is_more<T> : is_less<T>;

	for (size_t i = 1; i < vector.size(); ++i)
	{
		if (sort(vector[i - 1], vector[i]))
		{
			return false;
		}
	}

	return true;
}

template <class T>
vector_t<T> BozoSort(const vector_t<T>& vector, const bool& asc)
{
	vector_t<T> result = vector;
	if (vector.size() < 2)
	{
		return result;
	}

	const size_t size = result.size();
	while (!is_sorted(result, asc))
	{
		std::swap(result[std::rand() % size], result[std::rand() % size]);
	}
	return result;
}

struct student
{
private:
	static bool check_group(const uint32_t group)
	{
		return group >= 1 && group <= 9;
	}

	static bool check_grade(const uint32_t grade)
	{
		return grade >= 1 && grade <= 9;
	}

	static uint32_t validate_grade(const std::string& subject, const uint32_t grade)
	{
		if (!check_grade(grade))
		{
			throw std::invalid_argument(
				"Invalid " + subject + " grade. Value must be in range[1; 9]. Argument is: " + std::to_string(grade));
		}

		return grade;
	}

	static std::pair<std::string, uint32_t> get_pair(const std::string& subject, const uint32_t grade)
	{
		return { subject, validate_grade(subject, grade) };
	}

public:
	std::string _name;
	uint32_t _group;
	std::map<std::string, uint32_t> _exams;
	bool fail;

	student(const std::string& name, const uint32_t group, const uint32_t mathematics, const uint32_t physics, const uint32_t history, const uint32_t programming)
	{
		if (!check_group(group))
		{
			throw std::invalid_argument("Invalid group. Value must be in range [1; 9]. Argument is: " + std::to_string(group));
		}

		_exams =
		{
			get_pair(nameof(mathematics), mathematics),
			get_pair(nameof(physics), physics),
			get_pair(nameof(history), history),
			get_pair(nameof(programming), programming)
		};
		
		_name = name;
		_group = group;
		fail = mathematics <= 2 || physics <= 2 || history <= 2 || programming <= 2;
	}

	friend std::ostream& operator <<(std::ostream& stream, const student& student)
	{
		const auto length = std::max(student._name.length(), static_cast<size_t>(4));
		const std::string delemiter(length, '-');

		stream << "+-" << delemiter << "-+-------+------+------+------+------+\n"
			<< "| Name " << std::string(length - 4, ' ')
			<< "| Group | Math | Phys | Hist | Prog |\n"
			<< "|-" << delemiter << "-+-------+------+------+------+------+\n"
			<< "| " << student._name << " | " << student._group
			<< "     | " << student._exams.at("mathematics")
			<< "    | " << student._exams.at("physics")
			<< "    | " << student._exams.at("history")
			<< "    | " << student._exams.at("programming")
			<< "    |\n"
			<< "+-" << delemiter << "-+-------+------+------+------+------+\n";

		return stream;
	}

	bool operator <(const student& other) const
	{
		return _name < other._name;
	}

	bool operator >(const student& other) const
	{
		return _name > other._name;
	}
};

std::ostream& operator <<(std::ostream& stream, const std::vector<student>& students)
{
	auto length = students.at(0)._name.length();
	for (auto itr = students.begin() + 1; itr != students.end(); ++itr)
	{
		if (itr->_name.length() > length)
		{
			length = itr->_name.length();
		}
	}

	const std::string delemiter(length, '-');
	
	stream << "+-" << delemiter << "-+-------+------+------+------+------+\n"
		<< "| Name " << std::string(length - 4, ' ')
		<< "| Group | Math | Phys | Hist | Prog |\n"
		<< "+-" << delemiter << "-+-------+------+------+------+------+";

	for (const auto& student : students)
	{
		stream << "\n| " << student._name
			<< std::string(length - student._name.length(), ' ')
			<< " | " << student._group
			<< "     | " << student._exams.at("mathematics")
			<< "    | " << student._exams.at("physics")
			<< "    | " << student._exams.at("history")
			<< "    | " << student._exams.at("programming")
			<< "    |\n"
			<< "+-" << delemiter << "-+-------+------+------+------+------+";
	}
	
	stream << std::endl;

	return stream;
};

int main()
{
	std::srand(time(nullptr));
	setlocale(LC_ALL, "Russian");

	std::vector<student> students = BozoSort(std::vector<student>
	{
		student("Михаил", 5, 3, 3, 2, 5),
		student("Александр", 5, 3, 3, 3, 5),
		student("Алексей", 4, 3, 5, 2, 5),
		student("Светлана", 4, 3, 3, 4, 5),
		student("Владимир", 3, 3, 2, 3, 5),
		student("Виктор", 3, 4, 3, 4, 5),
		student("Мазай", 2, 4, 5, 3, 5),
		student("Артём", 2, 2, 5, 3, 5),
		student("Ольга", 1, 3, 3, 4, 5),
		student("Богдан", 1, 3, 4, 3, 5)
	}, true);

	std::vector<student> failed;
	for (auto& student : students)
	{
		if (student.fail)
		{
			failed.push_back(student);
		}
	}
	
	if (failed.empty())
	{
		std::cout << "Not found" << std::endl;
		return 0;
	}

	std::cout << failed << std::endl;
	std::cout << "Explusion\n" << failed.at(std::rand() % failed.size()) << std::endl;
}
