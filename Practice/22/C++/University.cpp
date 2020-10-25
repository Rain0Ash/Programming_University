#include <iostream>
#include <string>
#include <vector>


bool try_get_input(size_t& x, size_t& y)
{
	std::cin >> x >> y;
	if (std::cin.fail())
	{
		std::cin.clear();
		std::cin.ignore(32767, '\n');
		return false;
	}

	std::cin.ignore(32767, '\n');
	return true;
}

void try_read(size_t& x, size_t& y)
{
	while (!try_get_input(x, y))
	{
		std::cout << "Неверный ввод. Повторите попытку." << std::endl;
	}
}

const std::string maze[] =
{
	"####B######################",
	"# #       #   #      #    #",
	"# # # ###### #### ####### #",
	"# # # #       #   #       #",
	"#   # # ######### # ##### #",
	"# # # #   #       #     # #",
	"### ### ### ############# #",
	"# #   #     # #           #",
	"# # #   ####### ###########",
	"# # # #       # #         C",
	"# # ##### ### # # ####### #",
	"# # #     ### # #       # #",
	"#   ##### ### # ######### #",
	"######### ### #           #",
	"# ####### ### #############",
	"A           #   ###   #   #",
	"# ############# ### # # # #",
	"# ###       # # ### # # # #",
	"# ######### # # ### # # # F",
	"#       ### # #     # # # #",
	"# ######### # ##### # # # #",
	"# #######   #       #   # #",
	"# ####### ######### #######",
	"#         #########       #",
	"#######E############D######"
};

const size_t maze_width = maze[0].length();
const size_t maze_height = std::size(maze);

enum offset { up, down, left, right };

bool is_valid_position(const size_t& x, const size_t& y)
{
	return y < maze_height && x < maze_width && maze[y][x] != '#';
}

struct point
{
	bool operator ==(const point& value) const
	{
		return this->x == value.x && this->y == value.y;
	}

	bool operator !=(const point& value) const
	{
		return !(*this == value);
	}

	const int x;
	const int y;

public:
	point(const int x, const int y) : x(x), y(y)
	{
	}

	bool is_valid() const
	{
		return is_valid_position(x, y);
	}

	bool get_char(char& chr) const
	{
		if (!is_valid())
		{
			return false;
		}

		chr = maze[y][x];
		return true;
	}

	point offset(offset offset) const
	{
		switch (offset)
		{
		case up: return point(x, y - 1);
		case down: return point(x, y + 1);
		case left: return point(x - 1, y);
		case right: return point(x + 1, y);
		default: throw;
		}
	}
};

void maze_solver(std::vector<point>& visited, std::vector<char>& exits, const point& position)
{
	if (!position.is_valid())
	{
		return;
	}

	if (std::find(visited.begin(), visited.end(), position) != visited.end())
	{
		return;
	}

	visited = std::vector<point>(visited);

	visited.push_back(position);

	char pos_char;
	
	if (!position.get_char(pos_char))
	{
		return;
	}

	if (pos_char != ' ' && std::find(exits.begin(), exits.end(), pos_char) == exits.end())
	{
		exits.push_back(pos_char);
	}

	maze_solver(visited, exits, position.offset(right));
	maze_solver(visited, exits, position.offset(left));
	maze_solver(visited, exits, position.offset(down));
	maze_solver(visited, exits, position.offset(up));
}

void main()
{
	setlocale(LC_ALL, "Russian");

	std::cout << "Введите координаты" << std::endl;

	size_t x, y;

	try_read(x, y);

	if (!is_valid_position(x, y))
	{
		std::cout << "Не верные координаты" << std::endl;
		return;
	}

	auto visited = std::vector<point>();
	auto exits = std::vector<char>();

	maze_solver(visited, exits, point(x, y));

	if (exits.empty())
	{
		std::cout << "Выхода нет" << std::endl;
		return;
	}

	for (size_t i = 0; i < exits.size(); i++)
	{
		std::cout << exits[i];
		if (i < exits.size() - 1)
		{
			std::cout << " ";
		}
	}
}
