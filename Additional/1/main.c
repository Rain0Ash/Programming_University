// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <windows.h>

void clear(HANDLE handle)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(handle, &csbi))
    {
        return;
    }

    DWORD count;
    DWORD cells = csbi.dwSize.X * csbi.dwSize.Y;

    COORD position = { 0, 0 };
    if (!FillConsoleOutputCharacter(handle, (TCHAR)' ', cells, position, &count))
    {
        return;
    }

    if (!FillConsoleOutputAttribute(handle, csbi.wAttributes, cells, position, &count))
    {
        return;
    }

    SetConsoleCursorPosition(handle, position);
}

const char* get_executable_directory(const char** argv)
{
    const char* directory = argv[0];
    size_t length = strlen(directory);

    size_t exelength = strlen(strrchr(directory, '\\'));

    size_t size = length - exelength;
    char* path = (char*)calloc(size, sizeof(char));

    if (path == NULL)
    {
        return NULL;
    }

    strncpy(path, directory, size);

    return path;
}

int change_directory(const char* directory)
{
    return _chdir(directory) == 0;
}

void set_executable_as_current_directory(const char** argv)
{
    const char* directory = get_executable_directory(argv);

    if (directory == NULL)
    {
        return;
    }

    change_directory(directory);
    free(directory);
}

const char* read_file(const char* filepath)
{
    FILE* image = fopen(filepath, "r");

    if (!image)
    {
        return NULL;
    }

    fseek(image, 0L, SEEK_END);
    const long length = ftell(image);

    fseek(image, 0L, SEEK_SET);

    char* buffer = (char*)calloc(length, sizeof(char));

    if (!buffer)
    {
        return NULL;
    }

    fread(buffer, sizeof(char), length, image);
    fclose(image);

    return buffer;
}


COORD get_image_size(const char* image)
{
    short width = 0;
    short height = 0;

    while (image[width] != '\0' && width < SHRT_MAX)
    {
        if (image[width] == '\n')
        {
            break;
        }

        width++;
    }

    for (unsigned short i = 0; image[i] != '\0' && i < SHRT_MAX; i++)
    {
        if (image[i] == '\n')
        {
            height++;
        }
    }

    COORD size = { width, height };
    return size;
}

BOOL SetConsoleFontSize(const HANDLE handle, const unsigned short width, const unsigned short height)
{
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof cfi;
    cfi.nFont = 0;
    cfi.dwFontSize.X = width;
    cfi.dwFontSize.Y = height;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    wcscpy(cfi.FaceName, L"Consolas");

    return SetCurrentConsoleFontEx(handle, 0, &cfi);
}

int main(const int argc, const char** argv)
{
    set_executable_as_current_directory(argv);

    const HANDLE input_handle = GetStdHandle(STD_INPUT_HANDLE); //получаем дескриптор ввода консоли
    const HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE); //получаем дескриптор вывода консоли

    if (!handle || handle == INVALID_HANDLE_VALUE) //проверяем валидность дескриптора
    {
        printf("Console output handle is invalid!");
        exit(1);
    }

    clear(handle);

    DWORD mode; //объявляем переменную DWORD
    if (!GetConsoleMode(handle, &mode)) //получаем текущий режим консоли на вывод
    {
        printf("Can't get current console mode!");
        exit(1);
    }

    mode |= 0x0004 | 0x0008; //добавляем необходимые флаги

    if (!SetConsoleMode(handle, mode)) //устанавливаем новый режим терминала на вывод
    {
        printf("This windows version is not supported!");
        exit(1);
    }

    DWORD input_mode;
    if (GetConsoleMode(input_handle, &input_mode)) //получаем текущий режим консоли на ввод
    {
        input_mode |= 0x80;
        input_mode &= ~0x40;
        SetConsoleMode(input_handle, input_mode); //устанавливаем новый режим терминала на ввод
    }

    const char* filepath;
    if (argc > 1) //проверяем количество аргументов
    {
        filepath = argv[1]; //получаем путь к файлу
    }
    else
    {
        filepath = "image.txt"; //если путь не указан - используем стандартный
    }

    const char* content = read_file(filepath); //читаем файл
    if (!content)
    {
        printf("Can't read file");
        exit(1);
    }

    const COORD size_ansi = get_image_size(content); //получаем размер изображения

    SetConsoleFontSize(handle, 1, 1); //устанавливаем размер шрифта консоли равным 1
    SetConsoleScreenBufferSize(handle, size_ansi); //устанавливаем размер буфера консоли как размер изображения

    printf("%s", content); //выводим изображение

    free((void*)content); //очищаем данные из файла

    const COORD coordinate = { 0, 0 };
    SetConsoleCursorPosition(handle, coordinate); //перемещаем курсор на начальную позицию

    _getch(); //ждем нажатия клавиши перед завершением программы.

    return 0;
}
