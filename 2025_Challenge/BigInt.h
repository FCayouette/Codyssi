#pragma once
import std;

/////////////////////////////////////////////////////
// BigInt Class
//
// Implemented for Project Euler problems
// Optimized for string representation, not performance.


using u64 = unsigned long long;
class BigInt
{
public:
	BigInt() : decimalRep(1, 0) {}
	BigInt(const std::string& str)
	{
		decimalRep.reserve(str.size());
		std::transform(str.crbegin(), str.crend(), std::back_inserter(decimalRep), [](char c) { return c - '0'; });
	}
	BigInt(std::vector<char>&& other)
	{
		std::swap(decimalRep, other);
	}
	BigInt(const BigInt& r) : decimalRep(r.decimalRep) {}
	BigInt(BigInt&& r)
	{
		std::swap(decimalRep, r.decimalRep);
	}
	template<std::integral T>
	BigInt(T val)
	{
		if (val)
		{
			while (val)
			{
				T rem = val % (T)10;
				decimalRep.push_back((char)rem);
				val /= (T)10;
			}
		}
		else
			decimalRep.push_back(0);
	}
	BigInt& operator=(BigInt&& r)
	{
		std::swap(decimalRep, r.decimalRep);
		return *this;
	}

	BigInt operator+(const BigInt& r) const
	{
		std::vector<char> result;
		result.reserve(std::max(decimalRep.size(), r.decimalRep.size()) + 1);

		char remainder = 0;
		for (size_t i = 0, maxI = std::max(decimalRep.size(), r.decimalRep.size()); i < maxI; ++i)
		{
			remainder += (i < decimalRep.size() ? decimalRep[i] : 0) + (i < r.decimalRep.size() ? r.decimalRep[i] : 0);
			result.push_back(remainder % 10);
			remainder /= 10;
		}
		if (remainder > 0)
			result.push_back(remainder);

		return BigInt(std::move(result));
	}

	BigInt& operator+=(const BigInt& r)
	{
		char remainder = 0;
		size_t maxIndex = std::min(decimalRep.size(), r.decimalRep.size()), index = 0;
		for (; index < maxIndex; ++index)
		{
			remainder += decimalRep[index] + r.decimalRep[index];
			decimalRep[index] = remainder % 10;
			remainder /= 10;
		}
		if (decimalRep.size() > maxIndex)
		{
			while (remainder > 0)
			{
				if (index == decimalRep.size())
					decimalRep.push_back(0);
				remainder += decimalRep[index];
				decimalRep[index++] = remainder % 10;
				remainder /= 10;
			}
		}
		else
		{
			for (; index < r.decimalRep.size(); ++index)
			{
				remainder += r.decimalRep[index];
				decimalRep.push_back(remainder % 10);
				remainder /= 10;
			}
			if (remainder > 0)
				decimalRep.push_back(remainder);
		}

		return *this;
	}

	BigInt operator -(const BigInt& r) const
	{
		if (r >= *this) // No support for negative
			return { "0" };
		
		std::vector<char> result;
		result.reserve(decimalRep.size());
		bool borrow = false;
		size_t index = 0;
		while (borrow || index < r.decimalRep.size())
		{
			if (char rChar = index < r.decimalRep.size() ? r.decimalRep[index] : 0;
				rChar + borrow > decimalRep[index])
			{
				result.push_back(decimalRep[index] + 10 - (rChar + borrow));
				borrow = true;
			}
			else
			{
				result.push_back(decimalRep[index] - (rChar + borrow));
				borrow = false;
			}
			++index;
		}
		while (index < decimalRep.size())
			result.push_back(decimalRep[index++]);
		while (result.back() == 0)
			result.pop_back();

		return BigInt(std::move(result));
	}

	BigInt& operator-=(const BigInt& r)
	{
		if (r >= *this) // No support for negative
		{
			decimalRep.clear();
			decimalRep.push_back(0);
			return *this;
		}

		bool borrow = false;
		size_t index = 0;
		while (borrow || index < r.decimalRep.size())
		{
			char rChar = index < r.decimalRep.size() ? r.decimalRep[index] : 0;
			if (rChar + borrow > decimalRep[index])
			{
				decimalRep[index] = decimalRep[index] + 10 - (rChar + borrow);
				borrow = true;
			}
			else
			{
				decimalRep[index] -= (rChar + borrow);
				borrow = false;
			}
			++index;
		}
		while (decimalRep.back() == 0)
			decimalRep.pop_back();

		return *this;
	}

	BigInt operator*(const BigInt& r) const
	{
		return BigInt(std::move(InternalMul(r.decimalRep)));
	}

	BigInt& operator*=(const BigInt& r)
	{
		decimalRep = InternalMul(r.decimalRep);
		return *this;
	}

	BigInt operator^(const BigInt& r) const
	{
		std::vector<bool> exponent{ r.GetBitRepresentation() };

		BigInt result("1");
		BigInt mul(*this);
		for (bool b : exponent)
		{
			if (b)
				result *= mul;
			mul *= mul;
		}
		return result;
	}

	BigInt& operator^=(const BigInt& r)
	{
		std::vector<bool> exponent{ r.GetBitRepresentation() };

		BigInt mul("1");
		std::swap(decimalRep, mul.decimalRep);
		for (bool b : exponent)
		{
			if (b)
				*this *= mul;
			mul *= mul;
		}
		return *this;
	}

	constexpr std::string to_string() const
	{
		std::string result;
		result.reserve(decimalRep.size());
		std::transform(decimalRep.crbegin(), decimalRep.crend(), std::back_inserter(result), [](char c) { return c + '0'; });
		return result;
	}

	constexpr std::vector<bool> GetBitRepresentation() const // Index is the power of 2
	{
		std::vector<bool> rep;
		rep.reserve(decimalRep.size() * 3);
		std::vector<char> work(decimalRep);

		while (!work.empty())
		{
			rep.push_back((work[0] & 0x01) == 1);
			work[0] /= 2;
			for (size_t i = 1; i < work.size(); ++i)
			{
				if ((work[i] & 0x01) == 1)
					work[i - 1] += 5;
				work[i] /= 2;
			}
			if (work.back() == 0)
				work.pop_back();
		}

		if (rep.empty())
			rep.push_back(false);
		return rep;
	}

	constexpr size_t Size() const
	{
		return decimalRep.size();
	}

	void Swap(BigInt&& r)
	{
		std::swap(decimalRep, r.decimalRep);
	}

	bool operator <(const BigInt& r) const
	{
		if (decimalRep.size() == r.decimalRep.size())
			return std::lexicographical_compare(decimalRep.crbegin(), decimalRep.crend(), r.decimalRep.crbegin(), r.decimalRep.crend());
		return decimalRep.size() < r.decimalRep.size();
	}
	bool operator >(const BigInt& r) const {
		if (decimalRep.size() == r.decimalRep.size())
			return std::lexicographical_compare(decimalRep.crbegin(), decimalRep.crend(), r.decimalRep.crbegin(), r.decimalRep.crend(), std::greater());
		return decimalRep.size() > r.decimalRep.size();
	}

	bool operator==(const BigInt& r) const { return decimalRep == r.decimalRep; }
	bool operator!=(const BigInt& r) const { return decimalRep != r.decimalRep; }
	bool operator<=(const BigInt& r) const { return !(*this > r); }
	bool operator>=(const BigInt& r) const { return !(*this < r); }

	void Reverse()
	{
		std::reverse(decimalRep.begin(), decimalRep.end());
		while (decimalRep.back() == 0)
			decimalRep.pop_back();
	}

	constexpr bool IsPalindrome() const
	{
		if (decimalRep.size() < 2)
			return true;
		auto iter = decimalRep.begin(), iter2 = std::prev(decimalRep.end());
		while (std::distance(iter, iter2) >= 0)
			if (*(iter++) != *(iter2--))
				return false;
		return true;
	}

	constexpr size_t DigitSum() const
	{
		return std::accumulate<decltype(decimalRep.cbegin()), size_t>(decimalRep.cbegin(), decimalRep.cend(), 0ull);
	}

private:
	std::vector<char> InternalMul(const std::vector<char>& other) const
	{
		std::vector<char> result;
		result.reserve(decimalRep.size() + other.size() + 1);
		const std::vector<char>& big = (decimalRep.size() <= other.size() ? other : decimalRep);
		const std::vector<char>& small = (decimalRep.size() <= other.size() ? decimalRep : other);
		std::vector<std::vector<char>> intermediate(small.size());
		size_t maxSize = 0;
		for (size_t i = 0; i < small.size(); ++i)
		{
			intermediate[i].reserve(big.size() + i + 1);
			for (size_t j = 0; j < i; ++j)
				intermediate[i].push_back(0);
			u64 remainder = 0;
			for (size_t j = 0; j < big.size(); ++j)
			{
				remainder += small[i] * big[j];
				intermediate[i].push_back(remainder % 10ull);
				remainder /= 10ull;
			}
			while (remainder > 0)
			{
				intermediate[i].push_back(remainder % 10ull);
				remainder /= 10ull;
			}
			maxSize = std::max(maxSize, intermediate[i].size());
		}

		u64 remainder = 0ull;
		for (size_t j = 0; j < maxSize; ++j)
		{
			for (size_t i = 0; i < intermediate.size(); ++i)
				remainder += (j < intermediate[i].size() ? intermediate[i][j] : 0);
			result.push_back(remainder % 10ull);
			remainder /= 10ull;
		}
		while (remainder > 0)
		{
			result.push_back(remainder % 10ull);
			remainder /= 10ull;
		}

		return result;
	}

	std::vector<char> decimalRep; // reversed digits for easier computations
};

template <>
struct std::formatter<BigInt>
{
	constexpr auto parse(std::format_parse_context& ctx) //No formatting expected, ignore all
	{
		auto pos = ctx.begin();
		while (pos != ctx.end() && *pos != '}')
			++pos;
		return pos;
	}
	auto format(const BigInt& bi, std::format_context& ctx) const
	{
		return std::format_to(ctx.out(), "{}", bi.to_string());
	}
};