import std;

#define ALL(x) (x).begin(),(x).end()
#define ALLc(x) (x).cbegin(),(x).cend()
#define ALLr(x) (x).rbegin(),(x).rend()
#define ALLcr() (x).crbegin(),(x).crend()

using u64 = unsigned long long;
using i64 = long long;

struct Material
{
	int quality = {}, cost = {}, unique = {};
	constexpr auto operator <=>(const Material&) const = default;
};

struct Combination
{
	Combination() = default;
	Combination(const Material& m, int index) : quality(m.quality), unique(m.unique), cost(m.cost) { usedMaterials.insert(index); }
	std::set<int> usedMaterials;
	int quality = {}, unique = {}, cost = {};
	bool BetterThan(const Combination& c) const
	{
		return quality > c.quality || (quality == c.quality && unique < c.unique);
	}
	bool Disjoint(const Combination& c) const
	{
		auto l = c.usedMaterials.cbegin(), r = usedMaterials.cbegin();
		while (l != c.usedMaterials.cend() && r != usedMaterials.cend())
		{
			if (*l == *r) return false;
			if (*l < *r) ++l;
			else ++r;
		}
		return true;
	}
	Combination Merge(const Combination& c) const
	{
		Combination m = *this;
		m.quality += c.quality;
		m.cost += c.cost;
		m.unique += c.unique;
		m.usedMaterials.insert(c.usedMaterials.cbegin(), c.usedMaterials.cend());
		return m;
	}
};

int main(int argc, char* argv[])
{
	auto start = std::chrono::high_resolution_clock::now();
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

	i64 result1 = 0, result2 = 0, result3 = 0;
	int n, q, c, u;
	std::string _;
	std::vector<Material> work;
	while (in >> n >> _ >> _ >> _ >> _ >> q >>_ >> _ >> _ >> c >> _ >> _ >> _ >> _ >> u)
		work.emplace_back(q, c, u);
	
	std::sort(ALL(work), std::greater());
	result1 = std::accumulate(work.cbegin(), work.cbegin() + 5, 0, [](int t, const Material& m) {return t + m.unique; });
	std::sort(ALL(work), [](const Material& l, const Material& r)
		{ if (l.cost < r.cost) return true; if (l.cost > r.cost) return false;
	if (l.quality > r.quality) return true; if (l.quality < r.quality) return false; return l.unique < r.unique; });
	std::array<Combination, 301> combinations;
	for (int i = 1; i < combinations.size(); ++i)
	{
		Combination best;
		for (int m = 0; m < work.size() && work[m].cost <= i; ++m)
		{
			Combination c(work[m], m);
			if (c.BetterThan(best))
				best = c;
			for (int left = 1; left <= i - c.cost; ++left)
				if (c.Disjoint(combinations[left]))
					if (Combination m = combinations[left].Merge(c); m.BetterThan(best))
						best = m;
		}
		combinations[i] = best;
	}

	std::cout << std::format("Part 1: {}\nPart 2: {}\nPart 3: {}\n", result1, combinations[30].quality * combinations[30].unique, combinations[300].quality * combinations[300].unique);
	std::cout << std::format("Duration: {}\n", std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start));
	return 0;
}