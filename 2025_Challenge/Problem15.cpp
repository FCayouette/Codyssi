import std;

[[nodiscard]] std::pair<std::string, std::string> SplitAtFirst(std::string s, std::string token)
{
	auto pos = s.find(token);
	if (pos == std::string::npos)
		return std::make_pair(std::move(s), std::string{});
	return std::make_pair(s.substr(0, pos), s.substr(pos + token.size()));
}
template <typename T>
class y_combinator {
	T lambda;
public:
	constexpr y_combinator(T&& t) : lambda(std::forward<T>(t)) {}
	template <typename...Args>
	constexpr decltype(auto) operator()(Args&&...args) const { return lambda(std::move(*this), std::forward<Args>(args)...); }
};

struct Node
{
	int id = {}, left = -1, right = -1;
};

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: Problem15.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << std::format("Could not open {}\n", argv[1]);
		return 1;
	}

	int result1 = 0;
	std::string line, result2, result3;
	std::vector<Node> tree;
	std::vector<std::string> names;
	std::vector<std::vector<int>> layerValues;
	
	auto PlaceNode = y_combinator([&tree](auto&& PlaceNode, int look, int place, int depth) -> int
		{
			if (tree[look].id > tree[place].id)
			{
				if (tree[look].left == -1)
					tree[look].left = place;
				else return PlaceNode(tree[look].left, place, depth+1);
			}
			else
			{
				if (tree[look].right == -1)
					tree[look].right = place;
				else return PlaceNode(tree[look].right, place, depth+1);
			}
			return depth;
		});

	while (std::getline(in, line))
	{
		if (line.empty())
			break;
		auto [name, val] = SplitAtFirst(std::move(line), " | ");
		int id = std::stoi(val);
		tree.emplace_back(id);
		if (tree.size() > 1)
		{
			int depth = PlaceNode(0, (int)tree.size() - 1, 1);
			if (depth >= layerValues.size())
				layerValues.resize(depth + 1);
			layerValues[depth].push_back(id);
		}
		else
			layerValues.emplace_back(std::vector{ id });
		names.emplace_back(std::move(name));
	}

	for (const auto& layer : layerValues)
		result1 = std::max(result1, std::accumulate(layer.cbegin(), layer.cend(), 0));

	auto PrintPath = y_combinator([&tree, &names, &result2](auto&& PrintPath, int index, int ID) -> void
		{
			if (result2.empty())
				result2 = names[index];
			else
				result2 += "-" + names[index];
			if (ID != tree[index].id)
			{
				if (ID < tree[index].id)
				{
					if (tree[index].left >= 0)
						PrintPath(tree[index].left, ID);
				}
				else if (tree[index].right >= 0)
					PrintPath(tree[index].right, ID);
			}
		});

	in >> line >> line >> line;
	PrintPath(0, std::stoi(line));
	std::swap(result2, result3);
	in >> line >> line >> line;
	PrintPath(0, std::stoi(line));
	size_t pos = 0;
	while (pos < result3.size() && pos < result2.size() && result3[pos] == result2[pos])
		++pos;
	if (result3[--pos] == '-')
		--pos;
	result3.resize(pos+1);
	pos = result3.rfind('-');
	if (pos != std::string::npos)
		result3 = result3.substr(pos + 1);
	result2.clear();
	PrintPath(0, 500'000);
	std::cout << std::format("Part 1: {}\nPart 2: {}\nPart 3: {}\n", result1 * layerValues.size(), result2, result3);
	
	return 0;
}