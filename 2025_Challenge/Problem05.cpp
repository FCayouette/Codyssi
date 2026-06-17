import std;

#define ALL(x) (x).begin(),(x).end()

template<typename T>
struct Coord
{
	constexpr bool operator <  (const Coord& p) const { if (x < p.x) return true; else if (p.x < x) return false; else return y < p.y; }
	T x = {}, y = {};
};
using Point = Coord<int>;

template<typename T>
T ManhattanDistance(const Coord<T>& c1, const Coord<T>& c2)
{
	if constexpr (std::is_signed_v<T>)
		return std::abs(c1.x - c2.x) + std::abs(c1.y - c2.y);
	else
		return (c2.x < c1.x ? c1.x - c2.x : c2.x - c1.x) + (c2.y < c1.y ? c1.y - c2.y : c2.y - c1.y);
}

[[nodiscard]] std::pair<std::string, std::string> SplitAtFirst(std::string s, std::string token)
{
	auto pos = s.find(token);
	if (pos == std::string::npos)
		return std::make_pair(std::move(s), std::string{});
	return std::make_pair(s.substr(0, pos), s.substr(pos + token.size()));
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: Problem05.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << std::format("Could not open {}\n", argv[1]);
		return 1;
	}

	int result2 = 0, result3 = 0, minDist = std::numeric_limits<int>::max(), maxDist = 0;
	std::vector<std::pair<int, Point>> distances, next;
	std::string line;
	while (std::getline(in, line))
	{
		auto [x, y] = SplitAtFirst(line.substr(1, line.size() - 2), ", ");
		Point island(std::stoi(x), std::stoi(y));
		int dist = std::abs(island.x) + std::abs(island.y);
		minDist = std::min(minDist, dist);
		maxDist = std::max(maxDist, dist);
		distances.emplace_back(dist, island);
	}

	std::sort(ALL(distances));
	result3 = distances.front().first;
	while (distances.size() > 1)
	{
		next.clear();
		for (int i = 1; i < distances.size(); ++i)
			next.emplace_back(ManhattanDistance(distances.front().second, distances[i].second), distances[i].second);
		std::sort(ALL(next));
		result3 += next.front().first;
		if (!result2)
			result2 = next.front().first;
		std::swap(distances, next);
	}	

	std::cout << std::format("Part 1: {}\nPart 2: {}\nPart 3: {}\n", maxDist - minDist, result2, result3);
	return 0;
}