import std;

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: Problem01.exe inputFilename\n";
        return -1;
    }
    std::ifstream in(argv[1]);
    if (!in)
    {
        std::cout << std::format("Could not open {}\n", argv[1]);
        return 1;
    }

	std::string line;
	std::vector<int> values;
	while (true)
	{
		in >> line;
		if (char c = line[0]; c == '-' || c == '+')
			break;
		else values.push_back(c-'0');
	}

	int result1 = values.front(), result2 = result1, result3 = values.front() * 10 + values[1];
	for (int i = 0; i < line.size(); ++i)
	{
		result1 += (line[i] == '-' ? -1 : 1) * values[i + 1];
		result2 += (line[line.size() - 1 - i] == '-' ? -1 : 1) * values[i + 1];
	}
	for (int i = 1; i * 2 < line.size(); ++i)
		result3 += (values[i * 2] * 10 + values[i * 2 + 1]) * (line.cend()[-i] == '-' ? -1 : 1);

	std::cout << std::format("Part 1: {}\nPart 2: {}\nPart 3: {}\n", result1, result2, result3);
	return 0;
}
