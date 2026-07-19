import std;

template <typename T>
constexpr bool IsEqual(T a, T b)
{
	if constexpr (std::is_floating_point_v<T>)
		return (a < b ? b - a : a - b) < static_cast<T>(0.000001);
	else
		return a == b;
}

template<typename T>
struct Coord3D
{
	constexpr bool operator <  (const Coord3D& p) const { if (x < p.x) return true; if (p.x < x) return false; if (y < p.y) return true; if (p.y < y) return false; return z < p.z; }
	constexpr Coord3D operator + (const Coord3D& p) const { return Coord3D(x + p.x, y + p.y, z + p.z); }
	constexpr bool operator==(const Coord3D& p) const { return IsEqual(x, p.x) && IsEqual(y, p.y) && IsEqual(z, p.z); }
	T x = {}, y = {}, z = {};
};

constexpr std::array<Coord3D<int>, 7> AllDirections = { Coord3D{0,0,0}, Coord3D<int>(1, 0, 0), Coord3D<int>(0, 1, 0), Coord3D<int>(0, 0, 1), Coord3D<int>(-1, 0, 0), Coord3D<int>(0, -1, 0), Coord3D<int>(0, 0, -1) };

template<typename T>
struct Coord4D
{
	constexpr Coord4D(T X = 0, T Y = 0, T Z = 0, T W = 0) : x(X), y(Y), z(Z), w(W) {}
	constexpr bool operator <(const Coord4D& p) const { if (x < p.x) return true; if (p.x < x) return false; if (y < p.y) return true; if (p.y < y) return false; if (z < p.z) return true; if (p.z < z) return false; return w < p.w; }
	constexpr Coord4D& operator+=(const Coord4D& r) { x += r.x; y += r.y; z += r.z; w += r.w; return *this; }
	T x, y, z, w;
};

struct Debris
{
	Coord4D<int> pos, vel;
	std::optional<Coord3D<int>> Move()
	{
		pos += vel;
		if (pos.x < 0) pos.x += 10;
		else if (pos.x > 9) pos.x -= 10;
		if (pos.y < 0) pos.y += 15;
		else if (pos.y > 14) pos.y -= 15;
		if (pos.z < 0) pos.z += 60;
		else if (pos.z > 59) pos.z -= 60;
		if (pos.w < -1) pos.w += 3;
		else if (pos.w > 1) pos.w -= 3;
		if (!pos.w)
			return Coord3D<int>{ pos.x, pos.y, pos.z };
		return {};
	}
};

struct Ship
{
	Coord3D<int> pos = {};
	int hits = 0;

	constexpr bool operator <(const Ship& s) const { if (pos < s.pos) return true; if (s.pos < pos) return false; return hits < s.hits; }
};

bool InBounds(const Coord3D<int>& c)
{
	return c.x >= 0 && c.y >= 0 && c.z >= 0 && c.x < 10 && c.y < 15 && c.z < 60;
}

int main(int argc, char* argv[])
{
	auto chronoStart = std::chrono::high_resolution_clock::now();
	if (argc < 2)
	{
		std::cout << "Usage: Problem18.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << std::format("Could not open {}\n", argv[1]);
		return 1;
	}

	int result2 = 0, result3 = 0;
	std::string line;
	std::vector<Debris> debris;
	while (std::getline(in, line))
	{
		int r, x,y, z, a, div, rem, vx, vy, vz, va;
#pragma warning(suppress : 4996)
		std::sscanf(line.c_str(), "RULE %d: %dx+%dy+%dz+%da DIVIDE %d HAS REMAINDER %d | DEBRIS VELOCITY (%d, %d, %d, %d)", &r, &x, &y, &z, &a, &div, &rem, &vx, &vy, &vz, &va);
		for (int i = 0; i < 10; ++i)
			for (int j = 0; j<15; ++j )
				for (int k = 0; k < 60; ++k)
					for (int l = -1; l < 2; ++l)
					{
						int total = i * x + j * y + k * z + l * a;
						while (total < 0)
							total += div;
						if (total % div == rem)
							debris.emplace_back(Coord4D{ i, j, k, l }, Coord4D{ vx, vy, vz, va });
					}
	}
	Coord3D safe = { 0,0,0 }, start = safe, end = { 9,14,59 };
	std::set<Coord3D<int>> cur = {safe}, next;
	std::vector<std::map<Coord3D<int>, int>> costs;
	costs.resize(60);
	while (!cur.contains(end))
	{
		if (result2 < 60)
			for (Debris& d : debris)
				if (auto c = d.Move())
					++costs[result2][*c];
		int costIndex = result2++ % 60;
		for (const Coord3D<int>& pos : cur)
			for (Coord3D d : AllDirections)
				if (Coord3D np = pos + d; np == safe || !costs[costIndex].contains(np) && InBounds(np))
					next.insert(np);
		std::swap(cur, next);
		next.clear();
	}

	std::set<Ship> current, nxt;
	current.emplace(safe, 0);
	while (current.crbegin()->pos != end)
	{
		Coord3D last{ -1,0,0 };
		size_t costIndex = result3++ % 60;
		for (const Ship& s : current)
			if (s.pos != last)
			{
				last = s.pos;
				for (Coord3D d : AllDirections)
				{
					if (Coord3D np = s.pos + d; np == safe)
						nxt.emplace(np, 0);
					else if (InBounds(np))
					{
						int h = s.hits;
						if (auto iter = costs[costIndex].find(np); iter != costs[costIndex].cend())
							h += iter->second;
						if (h < 4)
							nxt.emplace(np, h);
					}
				}
			}
		std::swap(current, nxt);
		nxt.clear();
	}
	std::cout << std::format("Part 1: {}\nPart 2: {}\nPart 3: {}\n", debris.size(), result2, result3);
	std::cout << std::format("Duration: {}\n", std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - chronoStart));
	return 0;
}