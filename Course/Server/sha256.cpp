// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "sha256.hpp"
#include <cstring>
#include <sstream>
#include <iomanip>

sha256::sha256() : block_length_(0), bit_length_(0)
{
	state_[0] = 0x6a09e667;
	state_[1] = 0xbb67ae85;
	state_[2] = 0x3c6ef372;
	state_[3] = 0xa54ff53a;
	state_[4] = 0x510e527f;
	state_[5] = 0x9b05688c;
	state_[6] = 0x1f83d9ab;
	state_[7] = 0x5be0cd19;
}

sha256& sha256::update(const uint8_t* data, const size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		data_[block_length_++] = data[i];
		if (block_length_ == 64)
		{
			transform();

			bit_length_ += 512;
			block_length_ = 0;
		}
	}

	return *this;
}

sha256& sha256::update(const std::string& data)
{
	return update(reinterpret_cast<const uint8_t*>(data.c_str()), data.size());
}

uint8_t* sha256::digest()
{
	uint8_t* hash = new uint8_t[32];

	pad();
	revert(hash);

	return hash;
}

sha256& sha256::operator +=(const std::string& value)
{
	return *this << value;
}

sha256& sha256::operator <<(const std::string& value)
{
	return this->update(value);
}

uint32_t sha256::rotr(uint32_t x, uint32_t n)
{
	return x >> n | x << 32 - n;
}

uint32_t sha256::choose(uint32_t e, uint32_t f, uint32_t g)
{
	return e & f ^ ~e & g;
}

uint32_t sha256::majority(uint32_t a, uint32_t b, uint32_t c)
{
	return a & (b | c) | b & c;
}

uint32_t sha256::sig0(uint32_t x)
{
	return rotr(x, 7) ^ rotr(x, 18) ^ x >> 3;
}

uint32_t sha256::sig1(uint32_t x)
{
	return rotr(x, 17) ^ rotr(x, 19) ^ x >> 10;
}

void sha256::transform()
{
	uint32_t m[64];
	uint32_t state[8];

	for (uint8_t i = 0, j = 0; i < 16; i++, j += 4)
	{
		m[i] = data_[j] << 24 | data_[j + 1] << 16 | data_[j + 2] << 8 | data_[j + 3];
	}

	for (uint8_t k = 16; k < 64; k++)
	{
		m[k] = sig1(m[k - 2]) + m[k - 7] + sig0(m[k - 15]) + m[k - 16];
	}

	for (uint8_t i = 0; i < 8; i++)
	{
		state[i] = state_[i];
	}

	for (uint8_t i = 0; i < 64; i++)
	{
		const uint32_t maj = majority(state[0], state[1], state[2]);
		const uint32_t xor_a = rotr(state[0], 2) ^ rotr(state[0], 13) ^ rotr(state[0], 22);

		const uint32_t ch = choose(state[4], state[5], state[6]);

		const uint32_t xor_e = rotr(state[4], 6) ^ rotr(state[4], 11) ^ rotr(state[4], 25);

		const uint32_t sum = m[i] + array[i] + state[7] + ch + xor_e;
		const uint32_t new_a = xor_a + maj + sum;
		const uint32_t new_e = state[3] + sum;

		state[7] = state[6];
		state[6] = state[5];
		state[5] = state[4];
		state[4] = new_e;
		state[3] = state[2];
		state[2] = state[1];
		state[1] = state[0];
		state[0] = new_a;
	}

	for (uint8_t i = 0; i < 8; i++)
	{
		state_[i] += state[i];
	}
}

void sha256::pad()
{
	uint64_t i = block_length_;
	const uint8_t end = block_length_ < 56 ? 56 : 64;

	data_[i++] = 0x80;
	while (i < end)
	{
		data_[i++] = 0x00;
	}

	if (block_length_ >= 56)
	{
		transform();
		memset(data_, 0, 56);
	}

	bit_length_ += block_length_ * 8;
	data_[63] = bit_length_;
	data_[62] = bit_length_ >> 8;
	data_[61] = bit_length_ >> 16;
	data_[60] = bit_length_ >> 24;
	data_[59] = bit_length_ >> 32;
	data_[58] = bit_length_ >> 40;
	data_[57] = bit_length_ >> 48;
	data_[56] = bit_length_ >> 56;
	transform();
}

void sha256::revert(uint8_t* hash)
{
	for (uint8_t i = 0; i < 4; i++)
	{
		for (uint8_t j = 0; j < 8; j++)
		{
			hash[i + j * 4] = state_[j] >> 24 - i * 8 & 0x000000ff;
		}
	}
}

std::string sha256::to_string()
{
	return to_string(digest());
}

std::string sha256::to_string(const uint8_t* digest)
{
	std::stringstream s;
	s << std::setfill('0') << std::hex;

	for (uint8_t i = 0; i < 32; i++)
	{
		s << std::setw(2) << (unsigned int)digest[i];
	}

	return s.str();
}
