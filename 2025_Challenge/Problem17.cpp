import std;
#include "BigInt.h"

[[nodiscard]] std::vector<std::string> SplitAt(std::string s, std::string token)
{
	std::vector<std::string> results;
	if (s.empty()) [[unlikely]]
		return results;

	size_t tokenStart = 0;
	do
	{
		size_t e = s.find(token, tokenStart);
		if (e == std::string::npos)
		{
			results.emplace_back(s.substr(tokenStart));
			break;
		}
		results.emplace_back(s.substr(tokenStart, e - tokenStart));
		tokenStart = e + token.size();
	} while (tokenStart < s.size());

	return results;
}

struct Stair
{
	int start, end;
};

struct Connection
{
	int to, from;
};

struct TreeInfo
{
	int stair, index;
	constexpr bool operator<(const TreeInfo& r) const { if (stair < r.stair) return true; if (stair > r.stair) return false; return index < r.index; }
	constexpr bool operator!=(const TreeInfo& r) const { return stair != r.stair || index != r.index; }
};

struct Inputs
{
	inline static size_t LENGTH = 1;
	std::deque<std::vector<TreeInfo>> links;

	void Put(int stair, int index, size_t dist)
	{
		if (dist < LENGTH)
		{
			if (links.size() <= dist)
				links.resize(dist + 1);
			links[dist].emplace_back(stair, index);
		}
	}

	void Shift()
	{
		links.emplace_front();
		if (links.size() > LENGTH)
			links.pop_back();
	}

	void MergeFrom(const Inputs& other)
	{
		if (size_t required = std::min(LENGTH + 1, other.links.size() + 1); links.size() < required)
			links.resize(required);
		for (size_t i = 0; i< links.size()-1 && i < other.links.size(); ++i)
			links[i+1].insert(links[i+1].end(), other.links[i].cbegin(), other.links[i].cend());
	}
};

int main(int argc, char* argv[])
{
	auto timer_start = std::chrono::high_resolution_clock::now();
	if (argc < 2)
	{
		std::cout << "Usage: Problem17.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << std::format("Could not open {}\n", argv[1]);
		return 1;
	}

	std::string line, result3 = "S1_0";
	Stair base;
	std::vector<Stair> stairs;
	std::vector<std::vector<Connection>> connections;
	while (std::getline(in, line))
	{
		if (line.empty()) break;
		std::string name, _, s, e;
		int start, end;
		size_t index = stairs.size();
		std::stringstream ss(line);
		ss >> name >> _ >> start >> _ >> end >> _ >> _ >> s >> _ >> e;
		if (!index)
		{
			base = Stair(start, end);
			stairs.push_back(base);
			connections.resize(end + 1);
		}
		else
		{
			stairs.emplace_back(start, end);
			connections[start].emplace_back(index, std::stoi(s.substr(1)) - 1);
			connections[end].emplace_back(std::stoi(e.substr(1)) - 1, index);
		}

	}
	std::getline(in, line);
	auto pos = line.find(':');
	std::vector<int> moves, startingStairs;
	for (const std::string& s : SplitAt(line.substr(pos + 2), ", "))
		moves.push_back(std::stoi(s));

	Inputs::LENGTH = 1 + *std::max_element(moves.cbegin(), moves.cend());

	std::vector<BigInt> totalMoves;
	totalMoves.emplace_back("1");
	while (totalMoves.size() <= base.end)
	{
		BigInt total("0");
		for (int m : moves)
			if (m <= totalMoves.size())
				total += totalMoves[totalMoves.size() - m];
		totalMoves.emplace_back(std::move(total));
	}

	std::map<TreeInfo, std::set<TreeInfo>> forwardLinks;

	auto Count = [&](std::vector<std::vector<BigInt>>& maze, const Inputs& step, bool addForward = true)
		{
			std::set<TreeInfo> done;
			BigInt total("0");
			TreeInfo base = step.links[0][0];
			for (int m : moves)
				if (m < step.links.size())
					for (const TreeInfo ti : step.links[m])
						if (done.insert(ti).second)
						{
							total += maze[ti.stair][ti.index];
							if (addForward) forwardLinks[ti].insert(base);
						}

			maze[base.stair][base.index] = std::move(total);
		};

	auto CountPathsFrom = [&](const TreeInfo& start, bool populateForwardLinks = false) -> BigInt
		{
			std::vector<std::vector<BigInt>> maze(stairs.size(), std::vector<BigInt>(start.index + 1, { "0" }));
			maze[start.stair][start.index] = BigInt("1");
			std::vector<Inputs> cur(stairs.size());
			cur[start.stair].Put(start.stair, start.index, 0);

			std::vector<int> switches = { start.stair };
			// Check if this is an ending point
			if (stairs[start.stair].end == start.index)
				for (const Connection& c : connections[start.index])
					if (c.from == start.stair)
					{
						cur[c.to] = cur[start.stair];
						cur[c.to].Shift();
						cur[c.to].links[0].emplace_back(c.to, start.index);
						switches.push_back(c.to);
						Count(maze, cur[c.to], populateForwardLinks);
						break;
					}
			// Check starting stairs from initial point and or landing point
			for (const Connection& c : connections[start.index])
				if (std::find(switches.cbegin(), switches.cend(), c.from) != switches.cend())
				{
					cur[c.to] = cur[c.from];
					cur[c.to].Shift();
					cur[c.to].links[0].emplace_back(c.to, start.index);
					Count(maze, cur[c.to], populateForwardLinks);
				}

			for (size_t index = start.index + 1; index <= base.end; ++index)
			{
				for (int i = stairs.size() - 1; i >= 0; --i)
				{
					const Stair& curStair = stairs[i];
					maze[i].emplace_back("0");
					if (index == curStair.start) startingStairs.push_back(i); // will be processed later
					if (index <= curStair.start || index > curStair.end) continue;

					cur[i].Shift();
					cur[i].links[0].emplace_back(i, index);

					// Process incoming stairs, if any
					for (Connection c : connections[index])
						if (c.to == i)
							cur[i].MergeFrom(cur[c.from]);

					Count(maze, cur[i], populateForwardLinks);
				}

				// Now that connecting points have been processed, we can do starting stairs
				for (int starting : startingStairs)
				{
					for (Connection c : connections[index])
						if (c.to == starting)
						{
							cur[starting] = cur[c.from];
							cur[starting].Shift();
							cur[starting].links[0].emplace_back(starting, index);
						}
					Count(maze, cur[starting], populateForwardLinks);
				}
				startingStairs.clear();
			}
			return maze.front().back();
		};

	TreeInfo safe = { 0,0 }, end = { 0, base.end };
	BigInt result2 = CountPathsFrom(safe, true), target = { "100000000000000000000000000000" };
	while (safe != end)
	{
		bool found = false;
		for (const TreeInfo& ti : forwardLinks[safe])
		{
			BigInt pathsFrom = CountPathsFrom(ti);
			if (pathsFrom >= target)
			{
				found = true;
				safe = ti;
				break;
			}
			else
				target -= pathsFrom;
		}
		if (!found) // Too high of a target
			safe = *forwardLinks[safe].crbegin();
		
		result3 += std::format("-S{}_{}", safe.stair+1, safe.index);
	}

	std::cout << std::format("Part 1: {}\nPart 2: {}\nPart 3: {}\n", totalMoves.back(), result2, result3);
	std::cout << std::format("Duration: {}\n", std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - timer_start));
	return 0;
}