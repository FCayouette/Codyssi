import std;
#include "BigInt.h"

using i64 = long long;
#define SIZE 80

enum class What : char
{
	Face,
	Row,
	Col
};

struct Instruction
{
	size_t where;
	int value;
	What what;
};

struct Face
{
	Face() : orientation(0)
	{
		for (auto& v : values)
			v.fill(0);
	}
	std::array<std::array<int, SIZE>, SIZE> values;
	int orientation = 0;
};

struct Die
{
	std::array<Face, 6> faces;
	std::array<size_t, 6> facePositions = { 0, 1, 5, 4, 2, 3 }; // front, right, back, left, up, down

	constexpr static std::array<size_t, 4> tranverseFaces = { 0, 1, 4, 5 };
	constexpr static std::array<size_t, 4> sagittalFaces = { 0, 2, 3, 5 };
	constexpr static std::array<size_t, 4> coronalFaces = { 1, 2, 3, 4 };

	void Twist(char c)
	{
		if (c == 'R')
		{
			std::rotate(facePositions.begin(), std::next(facePositions.begin()), facePositions.begin() + 4);
			faces[facePositions[4]].orientation = (faces[facePositions[4]].orientation + 1) % 4;
			faces[facePositions[5]].orientation = (faces[facePositions[5]].orientation + 3) % 4;
		}
		else if (c == 'L')
		{
			std::rotate(facePositions.begin(), facePositions.begin()+3, facePositions.begin() + 4);
			faces[facePositions[4]].orientation = (faces[facePositions[4]].orientation + 3) % 4;
			faces[facePositions[5]].orientation = (faces[facePositions[5]].orientation + 1) % 4;
		}
		else if (c == 'U')
		{
			size_t oldFront = facePositions[0];
			facePositions[0] = facePositions[4];
			facePositions[4] = facePositions[2];
			facePositions[2] = facePositions[5];
			facePositions[5] = oldFront;
			faces[facePositions[2]].orientation = (faces[facePositions[2]].orientation + 2) % 4;
			faces[facePositions[4]].orientation = (faces[facePositions[4]].orientation + 2) % 4;
			faces[facePositions[1]].orientation = (faces[facePositions[1]].orientation + 3) % 4;
			faces[facePositions[3]].orientation = (faces[facePositions[3]].orientation + 1) % 4;
		}
		else
		{
			size_t oldFront = facePositions[0];
			facePositions[0] = facePositions[5];
			facePositions[5] = facePositions[2];
			facePositions[2] = facePositions[4];
			facePositions[4] = oldFront;
			faces[facePositions[2]].orientation = (faces[facePositions[2]].orientation + 2) % 4;
			faces[facePositions[5]].orientation = (faces[facePositions[5]].orientation + 2) % 4;
			faces[facePositions[1]].orientation = (faces[facePositions[1]].orientation + 1) % 4;
			faces[facePositions[3]].orientation = (faces[facePositions[3]].orientation + 3) % 4;
		}
	}

	void DoRow(size_t row, size_t face, int delta)
	{
		for (auto& v : faces[face].values[row])
			v = (v + delta) % 100;
	}

	void DoCol(size_t col, size_t face, int delta)
	{
		for (auto& row : faces[face].values)
			row[col] = (row[col] + delta) % 100;
	}

	void DoFace(size_t face, int delta)
	{
		for (auto& r : faces[face].values)
			for (auto& v : r)
				v = (v + delta) % 100;
	}

	void Apply(const Instruction& i)
	{
		if (i.what == What::Face)
			DoFace(facePositions[0], i.value);
		else
		{
			int orientation = (faces[facePositions[0]].orientation + (i.what == What::Col))%4;
			if (orientation & 0x01) // column
				DoCol((orientation == 1 ? i.where - 1 : SIZE - i.where), facePositions[0], i.value);
			else
				DoRow(!orientation ? i.where - 1 : SIZE - i.where, facePositions[0], i.value);
		}
	}

	void Wrap(const Instruction& i)
	{
		if (i.what == What::Face)
			DoFace(facePositions[0], i.value);
		else
		{
			int orientation = (faces[facePositions[0]].orientation + (i.what == What::Col)) % 4;
			if (orientation & 0x01 && std::binary_search(sagittalFaces.cbegin(), sagittalFaces.cend(), facePositions[0])) // column
			{
				// Sagittal
				size_t col = (orientation == 1 ? i.where - 1 : SIZE - i.where);
				constexpr static std::array<bool, 6> invert = { false, false, false, false, false, true }; // index 1 and 4 are unused
				bool faceInvert = invert[facePositions[0]];
				for (size_t face : sagittalFaces)
					DoCol(faceInvert == invert[face] ? col : SIZE-col-1, face, i.value);
			}
			else if (!(orientation & 0x01) && std::binary_search(tranverseFaces.cbegin(), tranverseFaces.cend(), facePositions[0]))
			{
				// Transverse
				size_t row = !orientation ? i.where - 1 : SIZE - i.where;
				for (size_t face : tranverseFaces)
					DoRow(row, face, i.value);
			}
			else
			{
				// Coronal
				size_t line = orientation < 2 ? i.where - 1 : SIZE - i.where;
				constexpr static std::array<bool, 6> invert = {false, true, false, true, false, false}; // index 0 and 5 are unused
				constexpr static std::array<bool, 6> useRow = {false, false, true, true, false, false}; // idem
				bool faceInvert = invert[facePositions[0]];
				for (size_t face : coronalFaces)
				{
					if (useRow[face])
						DoRow(faceInvert == invert[face] ? line : SIZE - line - 1, face, i.value);
					else
						DoCol(faceInvert == invert[face] ? line : SIZE - line - 1, face, i.value);
				}
			}
		}
	}

	i64 GetDominant(size_t face)
	{
		i64 result = 0;
		std::array<i64, SIZE> cols;
		cols.fill(0ll);
		for (const auto& row : faces[face].values)
		{
			i64 t = 0;
			for (size_t i = 0; i < SIZE; ++i)
			{
				cols[i] += row[i];
				t += row[i];
			}
			result = std::max(result, t);
		}

		return std::max(result, *std::max_element(cols.cbegin(), cols.cend()));
	}
};

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: Problem16.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << std::format("Could not open {}\n", argv[1]);
		return 1;
	}

	i64 result1 = 0;
	Die die, wrapping;
	std::array<i64, 6> absorptions;
	absorptions.fill(0ll);
	std::string line, twists;
	std::vector<Instruction> insts;
	while (std::getline(in, line))
	{
		if (line.empty()) break;
		std::stringstream ss(line);
		int where, value;
		if (char c = line[0]; c == 'F')
		{
			ss >> line >> line >> line >> value;
			insts.emplace_back(0ull, value, What::Face);
		}
		else
		{
			ss >> line >> where >> line >> line >> value;
			insts.emplace_back((size_t)where, value, c == 'R' ? What::Row : What::Col);
		}
	}
	in >> twists;

	for (size_t i = 0; i < insts.size(); ++i)
	{
		const auto& inst = insts[i];
		int power = inst.value * SIZE;
		if (inst.what == What::Face)
			power *= SIZE;
		absorptions[die.facePositions[0]] += power;
		die.Apply(inst);
		wrapping.Wrap(inst);

		if (i < twists.size())
		{
			die.Twist(twists[i]);
			wrapping.Twist(twists[i]);
		}
	}

	std::sort(absorptions.begin(), absorptions.end());
	i64 r2a = 1, r2b = 1, r3a = 1, r3b = 1;
	for (size_t i = 0; i < 6; i += 2)
	{
		r2a *= die.GetDominant(i) + 80;
		r2b *= die.GetDominant(i + 1) + 80;
		r3a *= wrapping.GetDominant(i) + 80;
		r3b *= wrapping.GetDominant(i + 1) + 80;
	}
	std::cout << std::format("Part 1: {}\nPart 2: {}\nPart 3: {}\n", absorptions[4] * absorptions[5], BigInt(r2a) * BigInt(r2b), BigInt(r3a) * BigInt(r3b));
	return 0;
}