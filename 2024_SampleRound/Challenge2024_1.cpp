import std.core;

using i64 = long long;

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: Challenge2024_1.exe inputFilename\n";
        return -1;
    }
    std::ifstream in(argv[1]);
    if (!in)
    {
        std::cout << std::format("Could not open {}\n", argv[1]);
        return 1;
    }

    i64 result = 0, part1 = 0, part3 = 0;
    std::vector<i64> values;
    i64 val;
    while (in >> val)
    {
        part1 += val;
        part3 += (values.size() % 2 == 0 ? val : -val);
        values.push_back(val);
    }

    std::sort(values.begin(), values.end());
    i64 part2 = std::accumulate(values.cbegin(), values.cend()-20, 0ll);

    std::cout << std::format("Part 1: {}\nPart 2: {}\nPart 3: {}\n", part1, part2, part3);
    return 0;
}

