import std;

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: ProblemXX.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << std::format("Could not open {}\n", argv[1]);
		return 1;
	}

	int result1 = 0, result2 = 0, result3 = 0, dist;
	std::string start, dest, _;
	std::unordered_map<std::string, size_t> nameMap;
	std::vector<std::vector<size_t>> graph, cycles, searches, next;
	std::vector<std::vector<std::pair<size_t, int>>> weighted;
	nameMap["STT"] = 0;
	auto GetIndex = [&nameMap](std::string s) -> size_t
		{
			if (auto iter = nameMap.find(s); iter != nameMap.cend())
				return iter->second;
			size_t result = nameMap.size();
			return nameMap[s] = result;
		};

	while (in >> start >> _ >> dest >> _ >> dist)
	{
		size_t s = GetIndex(start);
		if (s >= graph.size())
		{
			graph.resize(s + 1);
			weighted.resize(graph.size());
		}
		size_t d = GetIndex(dest);
		if (d >= graph.size())
		{
			graph.resize(d + 1);
			weighted.resize(graph.size());
		}
		graph[s].push_back(d);
		weighted[s].emplace_back(d, dist);
	}

	std::priority_queue<std::pair<int, size_t>, std::vector <std::pair<int, size_t>>, std::greater<std::pair<int, size_t>>> pqueue, wqueue;
	std::vector<int> hops(graph.size(), -1), paths(graph.size(), -1);
	pqueue.emplace(0, 0ull);
	while (!pqueue.empty())
	{
		auto [dist, target] = pqueue.top();
		pqueue.pop();
		if (hops[target] == -1)
		{
			hops[target] = dist;
			for (size_t n : graph[target])
				pqueue.emplace(dist + 1, n);
		}
	}
	std::sort(hops.begin(), hops.end());
	result1 = std::accumulate(hops.cend() - 3, hops.cend(), 1, std::multiplies());

	wqueue.emplace(0, 0ull);
	while (!wqueue.empty())
	{
		auto [dist, target] = wqueue.top();
		wqueue.pop();
		if (paths[target] == -1)
		{
			paths[target] = dist;
			for (auto [d, cost] : weighted[target])
				wqueue.emplace(dist + cost, d);
		}
	}
	std::sort(paths.begin(), paths.end());
	result2 = std::accumulate(paths.cend() - 3, paths.cend(), 1, std::multiplies());
	
	for (size_t s = 0; s < graph.size(); ++s)
		for (auto d : graph[s])
			if (s< d)
				searches.emplace_back(std::vector<size_t>{ s, d });

	while (!searches.empty())
	{
		for (const auto& a : searches)
			for (size_t n : graph[a.back()])
			{
				if (n > a.front() && std::find(a.cbegin(), a.cend(), n) == a.cend())
				{
					next.push_back(a);
					next.back().push_back(n);
				}
				else if (n == a.front())
				{
					cycles.push_back(a);
					cycles.back().push_back(n);
				}
			}
		std::swap(searches, next);
		next.clear();
	}

	for (const auto& c : cycles)
		result3 = std::max(result3, std::inner_product(c.cbegin(), std::prev(c.cend()), std::next(c.cbegin()), 0, std::plus(),
			[&weighted](size_t s, size_t d) { for (auto [dest, cost] : weighted[s]) if (dest == d) return cost; return 0; }));
	
	std::cout << std::format("Part 1: {}\nPart 2: {}\nPart 3: {}\n", result1, result2, result3);
	return 0;
}