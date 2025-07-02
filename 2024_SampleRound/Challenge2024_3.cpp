import std.core;

#define ALL(x) (x).begin(),(x).end()

using i64 = long long;

std::pair<std::string, std::string> SplitAtFirst(std::string s, std::string token)
{
	auto pos = s.find(token);
	if (pos == std::string::npos)
		return std::make_pair(std::move(s), std::string{});
	return std::make_pair(s.substr(0, pos), s.substr(pos + token.size()));
}

int main(int argc, char* argv[])
{
	auto ChronoStart = std::chrono::high_resolution_clock::now();
	if (argc < 2)
	{
		std::cout << "Usage: Challenge2024_3.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << std::format("Could not open inputFilename {}\n", argv[1]);
		return -1;
	}

	i64 part1 = 0, part2 = 0;
	std::string line, part3;

	while (std::getline(in, line))
	{
		const auto [s, b] = SplitAtFirst(line, " ");
		int base = stoi(b);
		part1 += base;
		part2 += stoi(s, nullptr, base);
	}
	i64 total = part2;
	constexpr std::string_view encoding = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!@#";
	while (total > 0)
	{
		std::lldiv_t div = std::lldiv(total, 65ll);
		part3 += encoding[div.rem];
		total = div.quot;
	}

	std::reverse(ALL(part3));

	std::cout << std::format("Part 1: {}\nPart 2: {}\nPart 3: {}\n", part1, part2, part3);
	std::cout << std::format("Duration: {}\n", std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - ChronoStart));
}