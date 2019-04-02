#ifndef _BIGINT_H
#define _BIGINT_H

#include <iostream>
#include <limits>
#include <math.h>

template <typename T> int sgn(T val) 
{
	if (val == 0) // не был уверен, какой знак должен быть у нуля
		return 1;
    return (T(0) < val) - (val < T(0));
}

uint16_t len_of_num(int64_t num)
// кол-во цифр в числе
{
	uint16_t counter = 1;
	while (num != 0)
	{
		num /= 10;
		counter ++;
	}
	return counter - 1;
}


class BigInt
{
public:

	friend std::ostream& operator << (std::ostream& out, const BigInt& value);

	BigInt () : BigInt(0) {}

	BigInt(int64_t number) 
	// число хранится в системе счисления по основанию 10^9, порядок little-endian
		: _power_of_base (9)
		, _sign (static_cast<int8_t>(sgn(number)))
	{
		number = abs(number);
		_base = static_cast<uint32_t>(pow(10, _power_of_base));
		_bits_number = len_xbase(number);
		_bits = new int32_t[_bits_number];

		for (size_t i = 0; i < _bits_number; i++)
		{
			_bits[i] = number % _base;
			number = number / _base;
		}
	}

	~BigInt()
	{
		delete[] _bits;
	}

	BigInt(const BigInt& other)
		: _base (other._base)
		, _power_of_base (other._power_of_base)
		, _sign (other._sign)
		, _bits_number (other._bits_number)
	{
		_bits = new int32_t[_bits_number];

		for (size_t i = 0; i < _bits_number; i++)
			_bits[i] = other._bits[i];
	}

	BigInt& operator= (const BigInt& other)
	{
		_power_of_base = other._power_of_base;
		_base = other._base;
		_bits_number = other._bits_number;
		delete[] _bits;
		_bits = new int32_t[_bits_number];
		std::copy(other._bits, other._bits + _bits_number, _bits);
		return *this;
	}

	BigInt operator + (const BigInt& other) const
	{
		// если складываются 2 положительных числа или 2 отрицательных числа 
		// складываем модули, а знак сложения == знаку слагаемых
		if (_sign == other._sign)
		{
			if (other._bits_number > _bits_number)
			{
				BigInt this_copy = *this;
				return other + this_copy;
			}

			// сложение производится с учетом что у второго слагаемого меньше блоков

			BigInt temp = *this; // ЗНАК СОХРАНЯЕТСЯ
								// складывали два полож => в рез-те положительное
								// два отрицательных => отрицательное

			int carry = 0;

			//попарное сложение всех блоков
			for(size_t i = 0; i < other._bits_number; i++)
			{
				temp._bits[i] = temp._bits[i] + other._bits[i] + carry;
				carry = 0;
				if (temp._bits[i] >= _base)
				{
					temp._bits[i] = temp._bits[i] % _base;
					carry = 1;
				}
			}

			//результат сложения во всех блоках надо привести по модулю _base с переносом
			size_t j = other._bits_number;
			while (carry == 1 && j < temp._bits_number)
			{
				temp._bits[j] += 1;
				carry = 0;
				if (temp._bits[j] >= _base)
				{
					temp._bits[j] = temp._bits[j] % _base;
					carry = 1;
				}

				j += 1;
			}

			if (carry == 1) 	// если переполнение в самом старшем разряде, то придется вручную добавить новый разряд
			{
				BigInt temp2 = temp;
				delete[] temp2._bits;
				temp2._bits_number += 1;
				temp2._bits = new int32_t[temp2._bits_number];
				for (size_t i = 0; i < temp2._bits_number - 1; i++)
					temp2._bits[i] = temp._bits[i];
				temp2._bits[temp2._bits_number - 1] = 1;
				return temp2;
			}	
			return temp;
		}
		if (_sign == 1 && other._sign == -1)
			return *this - (-other);
		// _sign == -1 && other._sign == 1
		return other - (- *this); 

		
	}

	BigInt operator - (const BigInt& other) const
	{
		if (_sign == 1 && other._sign == 1)
		{
			// случай положительных чисел !!!
			if (*this < other)
			{
				BigInt this_copy = other - *this;
				this_copy._sign = -1;
				return this_copy;
			} 
			// из большего числа вычитается положительное
			BigInt temp = *this;
			for (size_t i = 0; i < other._bits_number; i++)
				temp._bits[i] -= other._bits[i];

			int carry = 0;
			for (size_t i = 0; i < temp._bits_number; i++)
			{
				temp._bits[i] -= carry;
				carry = 0;
				if (temp._bits[i] < 0)
				{
					temp._bits[i] = temp._bits[i] + _base - carry;
					carry = 1;
				}
			}

			return temp;
		}

		if (_sign == -1 && other._sign == -1) // -this - (-other) = other - this
			return -other - (- *this);
	
		if (_sign == 1 && other._sign == -1)
			return *this + (-other);
		else 	// if (_sign == -1 && other._sign == 1)
			return *this + (-other);
	} 

	BigInt operator - () const
	{
		if (_bits[0] == 0 && _bits_number == 1) // если число 0, не меняем его знак
			return *this;
		BigInt temp = *this;
		_sign == 1 ? temp._sign = -1 : temp._sign = 1;
		return temp;
	}


	bool operator == (const BigInt& other) const
	{
		if (_sign != other._sign || _bits_number != other._bits_number)
			return false;
		int16_t i = _bits_number - 1;
		while (i >= 0)
		{
			if (_bits[i] != other._bits[i])
				return false;
			i --;
		}
		return true;
	}

	bool operator != (const BigInt& other) const
	{
		return !(*this == other);
	}


	bool operator > (const BigInt& other) const
	{
		if (_sign > other._sign)
			return true;
		if (_sign < other._sign)
			return false;
		if (_bits_number > other._bits_number)
		{
			if (_sign == 1)
				return true;
			if (_sign == -1)
				return false;
		}
		if (_bits_number < other._bits_number)
		{
			if (_sign == 1)
				return false;
			if (_sign == -1)
				return true;
		}
		// если одинаковые знаки и количество блоков
		int16_t i = _bits_number - 1;
		while(i >= 0)
		{
			if (_bits[i] > other._bits[i])
			{
				if (_sign == 1)
					return true;
				else
					return false; 
			}
			if (_bits[i] < other._bits[i])
			{
				if (_sign == 1)
					return false;
				else
					return true;
			}
			else
				i --;
		}
		return false;

	}

	bool operator >= (const BigInt& other) const
	{
		return (*this > other || *this == other);
	}

	bool operator < (const BigInt& other) const
	{
		return !(*this >= other);
	}

	bool operator <= (const BigInt& other) const
	{
		return !(*this > other);
	}






private:
	uint16_t _power_of_base;
	uint32_t _base;
	int8_t _sign;
	uint8_t _bits_number;
	int32_t *_bits;
	

	uint8_t len_xbase(int64_t num) // определяет сколько блоков bits поребуется для записи числа
	{
		if (num == 0)
			return 1;
		uint8_t counter = 0;
		while (num != 0)
		{
			num /= _base;
			counter ++;
		}
		return counter;
	}

};


std::ostream& operator<< (std::ostream& out, const BigInt& value)
{
	out << ((value._sign == 1) ? "" : "-");
	// старший разряд записывается как есть
	out << value._bits[value._bits_number - 1];

	// все остальные разряды дополняются нулями до размера _power_of_base
	for (size_t i = value._bits_number - 1; i > 0; i--)
	{
		if (value._bits[i-1] == 0)
		{
			for(size_t i = 0; i < value._power_of_base; i ++)
				out << 0;
		}
		else
		{
			uint16_t size = len_of_num(value._bits[i-1]);
			out << static_cast<uint32_t>(value._bits[i-1] * pow(10, value._power_of_base - size));
		}
	}
	
	return out;
}


#endif 
