#include <iostream>
#include <string>

void main()
{
    setlocale(LC_ALL, "Russian");

    std::cout << "Введите слово." << std::endl;

    std::string secret;

    std::getline(std::cin, secret);

    const std::string words[] = { "hallo", "klempner", "das", "ist", "fantastisch", "fluggegecheimen" };

    int chars[26] = {};

    const int a = 'a';

    for (const std::string& word : words)
    {
        for (const char& chr : word)
        {
            ++chars[chr - a];
        }
    }

    long double chance = 1;
    long double total = 0;

    for (const int& item : chars)
    {
        total += item;
    }

    for (const char& chr : secret)
    {
        chance *= chars[chr - a] / total;
        if (chance < LDBL_EPSILON) break;
    }
    
    std::cout.precision(16);
    std::cout << chance << std::endl;
}
