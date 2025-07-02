import std.core;

#define ALL(x) (x).begin(),(x).end()
#define ALLc(x) (x).cbegin(),(x).cend()
#define ALLr(x) (x).rbegin(),(x).rend()
#define ALLcr(x) (x).crbegin(),(x).crend()

using u64 = unsigned long long;
using i64 = long long;

int main(int argc, char* argv[])
{
	auto ChronoStart = std::chrono::high_resolution_clock::now();
	if (argc < 2)
	{
		std::cout << "Usage: Codyssi_X.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << std::format("Could not open inputFilename {}\n", argv[1]);
		return -1;
	}

	u64 part1 = 0, part2 = 0, part3 = 0;
	std::string line;
	std::vector<char> values, nextlayer;

	while (in >> line)
	{
		bool val = line[0] == 'T';
		values.push_back(val);
		if (val)
		{
			part1 += values.size();
			++part3;
		}
	}

	for (int i = 0; i < values.size(); i += 4)
	{
		char v = std::min(values[i], values[i + 1]);
		nextlayer.push_back(v);
		v = std::max(values[i+2], values[i+3]);
		nextlayer.push_back(v);
	}
	
	part3 += (part2 = std::accumulate(ALLc(nextlayer), 0));

	while (nextlayer.size() >= 2)
	{
		std::swap(nextlayer, values);
		nextlayer.clear();
		for (int i = 0; i < values.size(); i += 4)
		{
			char v = std::min(values[i], values[i + 1]);
			nextlayer.push_back(v);
			if (values.size() > 2)
			{
				v = std::max(values[i + 2], values[i + 3]);
				nextlayer.push_back(v);
			}
		}
		part3 += std::accumulate(ALLc(nextlayer), 0);
	}

	std::cout << std::format("Part 1: {}\nPart 2: {}\nPart 3: {}\n", part1, part2, part3);
	std::cout << std::format("Duration: {}\n", std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - ChronoStart));
}