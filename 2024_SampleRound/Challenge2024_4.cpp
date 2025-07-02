import std.core;

using u64 = unsigned long long;

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
		std::cout << "Usage: Codyssi_X.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << std::format("Could not open inputFilename {}\n", argv[1]);
		return -1;
	}

	u64 part2 = 0, part3 = 0, hours = 1;
	std::string line;
	std::map<std::string, std::vector<std::string>> connections;
	while (std::getline(in, line))
	{
		auto [a,b] = SplitAtFirst(line, " <-> ");
		connections[a].push_back(b);
		connections[b].push_back(a);
	}

	std::set<std::string> reached = {"STT"}, cur = reached, next;
	while (!cur.empty() && reached.size() != connections.size())
	{
		for (const std::string& c : cur)
			for (const std::string& n : connections[c])
				if (!reached.contains(n))
				{
					next.insert(n);
					reached.insert(n);
				}
		part3 += hours * next.size();
		if (hours++ == 3)
			part2 = reached.size();
		std::swap(cur, next);
		next.clear();
	}
	std::cout << std::format("Part 1: {}\nPart 2: {}\nPart 3: {}\n", connections.size(), part2, part3);
	std::cout << std::format("Duration: {}\n", std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - ChronoStart));
}