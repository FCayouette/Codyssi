import std;

using i64 = long long;

enum class Op : char
{
	Shift,
	Add,
	Sub,
	Multiply,
};
enum class Target : char
{
	Row,
	Col,
	All
};
struct Inst
{
	Op op;
	int val;
	Target what;
	int target;
};

enum class Flow : char
{
	Act,
	Cycle,
	Take,
};

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: Problem12.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << std::format("Could not open {}\n", argv[1]);
		return 1;
	}

	i64 result1 = 0, result2 = 0, result3 = 0, RANGE = 1073741824;
	std::string line;
	std::vector<std::vector<i64>> map, backup;
	std::deque<Inst> instructions;
	std::vector<Flow> flow;
	while (std::getline(in, line))
	{
		if (line.empty()) break;
		std::stringstream ss(line);
		std::vector<i64> row;
		int v;
		while (ss >> v)
			row.push_back(v);
		map.emplace_back(std::move(row));
	}

	while (std::getline(in, line))
	{
		if (line.empty()) break;
		std::stringstream ss(line);
		std::string s, _;
		ss >> s;
		if (s == "SHIFT")
		{
			int v, t;
			ss >> s >> t >> _ >> v;
			instructions.emplace_back(Op::Shift, v, s[0] == 'C' ? Target::Col : Target::Row, t-1);
		}
		else
		{
			int v, t, c = s[0];
			ss >> v >> s;
			if (s[0] != 'A')
				ss >> t;
			Op op = (c == 'A') ? Op::Add : ((c == 'M') ? Op::Multiply : Op::Sub);
			if (s[0] == 'A')
				instructions.emplace_back(op, v, Target::All);
			else instructions.emplace_back(op, v, s[0] == 'C' ? Target::Col : Target::Row, t-1);
		}
	}

	while (in >> line)
		flow.push_back(line[0] == 'A' ? Flow::Act : (line[0] == 'C' ? Flow::Cycle : Flow::Take));
	
	auto Act = [&map, RANGE](Inst i) -> void
	{
		if (i.op == Op::Shift)
		{
			if (i.what == Target::Row)
				std::rotate(map[i.target].begin(), map[i.target].end() - i.val, map[i.target].end());
			else
			{
				std::vector<i64> work;
				work.reserve(map.size());
				for (auto& r : map)
					work.push_back(r[i.target]);
				for (int x = 0; x < map.size(); ++x)
					map[x][i.target] = work[(work.size() + x - i.val) % work.size()];
			}
		}
		else
		{
			int val = i.op == Op::Sub ? -i.val : i.val;
			if (i.op == Op::Multiply)
			{
				if (i.what == Target::All)
					for (auto& r : map)
						for (i64& v : r)
							v = (v * val) % RANGE;
				else if (i.what == Target::Row)
					for (i64& v : map[i.target])
						v = (v * val) % RANGE;
				else
					for (auto& r : map)
						r[i.target] = (r[i.target] * val) % RANGE;
			}
			else
			{
				if (i.what == Target::All)
					for (auto& r : map)
						for (i64& v : r)
						{
							if (i64 r = v + val; r < 0)
								v = r + RANGE;
							else
								v = r % RANGE;
						}
				else if (i.what == Target::Row)
					for (i64& v : map[i.target])
					{
						if (i64 r = v + val; r < 0)
							v = r + RANGE;
						else
							v = r % RANGE;
					}
				else
					for (auto& r : map)
					{
						i64& v = r[i.target];
						if (i64 a = v + val; a < 0)
							v = a + RANGE;
						else
							v = a % RANGE;
					}
			}
		}
	};
	// Part 1
	backup = map;
	for (Inst i : instructions)
		Act(i);

	auto FindMax = [&map]() -> i64
		{
			std::vector<i64> totals(map.front().size(), 0ll);
			i64 result = 0ll;
			for (const auto& row : map)
			{
				i64 total = 0;
				for (size_t i = 0; i < row.size(); ++i)
				{
					totals[i] += row[i];
					total += row[i];
				}
				result = std::max(result, total);
			}
			return std::max(result, *std::max_element(totals.cbegin(), totals.cend()));
		};

	result1 = FindMax();
	
	// Part 2 and 3
	map = backup;
	Inst taken;
	while (!instructions.empty())
	{
		for (Flow f : flow)
		{
			if (f == Flow::Take)
			{
				if (instructions.empty())
					break;
				taken = instructions.front();
				instructions.pop_front();
			}
			else if (f == Flow::Cycle)
				instructions.push_back(taken);
			else
				Act(taken);
		}
		if (!result2)
			result2 = FindMax();
	}

	std::cout << std::format("Part 1: {}\nPart 2: {}\nPart 3: {}\n", result1, result2, FindMax());
	return 0;
}