import std;

using i64 = long long;

constexpr int ToDecimalValue(char c)
{
	if (c >= 'a' && c <= 'z')
		return c - 'a' + 36;
	if (c >= 'A' && c <= 'Z')
		return c - 'A' + 10;
	return c - '0';
}

constexpr char ToBase68(i64 val)
{
	if (val < 10)
		return '0' + val;
	if (val < 36)
		return 'A' + val - 10;
	if (val < 62)
		return 'a' + val - 36;
	switch (val)
	{
	case 62: return '!';
	case 63: return '@';
	case 64: return '#';
	case 65: return '$';
	case 66: return '%';
	default: return '^';
	}
}
constexpr int DigitsInBase(i64 val, i64 base)
{
	int result = 0;
	while (val > 0)
	{
		++result;
		val /= base;
	}
	return result;
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: Problem11.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << std::format("Could not open {}\n", argv[1]);
		return 1;
	}

	i64 result1 = 0, total = 0, result3 = 0;
	std::string number, result2;
	int base;
	while (in >> number >> base)
	{
		i64 val = 0;
		for (char c : number)
			val = val * base + ToDecimalValue(c);
		total += val;
		result1 = std::max(val, result1);
	}
	
	result3 = (i64)std::ceil(std::sqrt(std::sqrt((double)total)));
	while (total > 0ll)
	{
		auto dq = std::div(total, 68ll);
		result2 += ToBase68(dq.rem);
		total = dq.quot;
	}
	std::reverse(result2.begin(), result2.end());
	std::cout << std::format("Part 1: {}\nPart 2: {}\nPart 3: {}\n", result1, result2, result3);
	return 0;
}