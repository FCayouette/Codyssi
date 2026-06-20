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

	int result1 = std::numeric_limits<int>::max(), result2 = 0, rowCount = 0;
	std::string line;
	std::vector<int> prev, total;
	while (std::getline(in, line))
	{
		std::stringstream ss(line);
		std::vector<int> row, cur;
		int threat;
		while (ss >> threat)
			row.push_back(threat);
		result1 = std::min(result1, std::accumulate(row.cbegin(), row.cend(), 0));
		if (!rowCount++)
		{
			total = row;
			std::partial_sum(row.cbegin(), row.cend(), std::back_inserter(prev));
		}
		else
		{
			cur.push_back(prev.front() + row[0]);
			total[0] += row[0];
			for (int j = 1; j < prev.size(); ++j)
			{
				cur.push_back(std::min(cur.back(), prev[j]) + row[j]);
				total[j] += row[j];
			}
			if (rowCount == 15)
				result2 = cur[14];
			std::swap(cur, prev);
		}
	}
	std::cout << std::format("Part 1: {}\nPart 2: {}\nPart 3: {}\n", std::min(result1, *std::min_element(total.cbegin(), total.cend())), result2, prev.back());
	return 0;
}