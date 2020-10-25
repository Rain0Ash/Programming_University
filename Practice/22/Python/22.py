maze = (
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
)

maze_width = len(maze[0])
maze_height = len(maze)


def is_valid_position(x: int, y: int) -> bool:
    return y >= 0 and y < maze_height and x >= 0 and x < maze_width and maze[y][x] != '#';


def get_char(x: int, y: int) -> str:
    if is_valid_position(x, y):
        return maze[y][x]

    return None


def maze_solver(visited: list, exits: list, x, y):
    if not is_valid_position(x, y):
        return

    t = (x, y)
    if t in visited:
        return

    visited = [v for v in visited]
    visited.append(t)

    char = get_char(x, y)

    if char is None:
        return

    if char != ' ' and char not in exits:
        exits.append(char)

    maze_solver(visited, exits, x + 1, y)
    maze_solver(visited, exits, x - 1, y)
    maze_solver(visited, exits, x, y + 1)
    maze_solver(visited, exits, x, y - 1)


while True:
    try:
        x, y = map(int, input("Введите координаты x,y через пробел: ").split())

        if not is_valid_position(x, y):
            print("Не верные координаты")
            break

        exits = []

        maze_solver([], exits, x, y)

        if exits:
            print(' '.join(exits))
        else:
            print("Выхода нет")

        break

    except KeyboardInterrupt:
        break
    except ValueError:
        print("Ошибка парсинга значения. Повторите ввод.")
