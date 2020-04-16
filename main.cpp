#include <iostream>
#include <vector>
#include <string>
#include <time.h>

struct Vec2
{
	int x, y;

	Vec2() : x(-1), y(-1) {};
	Vec2(int x, int y) : x(x), y(y) {};

	friend Vec2 operator + (const Vec2& rhs, const Vec2& other)
	{
		return Vec2(rhs.x + other.x, rhs.y + other.y);
	}

	friend Vec2 operator - (const Vec2& rhs, const Vec2& other)
	{
		return Vec2(rhs.x - other.x, rhs.y - other.y);
	}
};

struct Cell
{
	Vec2 pos;

	bool
		visited = false,
		connSouth = false,
		connEast = false;

	Cell(const Vec2& pos) : pos(pos) {};
};

struct Maze
{
	const Vec2 size;
	std::vector<Cell> cells;

	Maze(const Vec2& size) : size(size)
	{
		// Construct solid maze before generating paths

		cells.reserve(size.x * size.y);

		for (int x = 0; x < size.x; x++)
			for (int y = 0; y < size.y; y++)
				cells.emplace_back(Cell(Vec2(x, y)));
	}

	Cell& getCell(Vec2 pos)
	{
		return cells[pos.x * size.y + pos.y];
	}

	// Update connections depending on relation between pos and prevPos
	void visitCell(const Vec2& pos, const Vec2& prevPos)
	{
		Cell& cell = getCell(pos);
		Cell& prevCell = getCell(prevPos);

		cell.visited = true;

		const Vec2 diff = pos - prevPos;

		if (diff.x < 0)			cell.connEast = true;
		else if (diff.x > 0)	prevCell.connEast = true;
		if (diff.y < 0)			cell.connSouth = true;
		else if (diff.y > 0)	prevCell.connSouth = true;
	}
};

bool canExtendPath(Maze& maze, const Vec2& pos)
{
	// Outside range?
	if (pos.x < 0 || pos.x >= maze.size.x || pos.y < 0 || pos.y >= maze.size.y) return false;

	// Visited?
	if (maze.getCell(pos).visited) return false;

	return true;
}

void recurExtendPath(Maze& maze, const Vec2& pos, const Vec2& prevPos)
{
	// Check if this cell can be path
	if (canExtendPath(maze, pos))
	{
		maze.visitCell(pos, prevPos);

		// Check every direction and if we can, we move there
		std::vector<Vec2> dirs = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} };

		for (int i = 0; i < 4; i++)
		{
			// Pick a random direction and then delete it from list so that it won't get picked again
			const int ri = rand() % dirs.size();
			const Vec2 randDir = dirs[ri];
			dirs.erase(dirs.begin() + ri);

			recurExtendPath(maze, pos + randDir, pos);
		}
	}
}

void print(Maze& maze)
{
	const Vec2 outputSize((maze.size.x * 2 + 1), (maze.size.y * 2 + 1));
	std::vector<char> output(outputSize.x * outputSize.y, 219);

	const auto& set = [&](const Vec2& pos)
	{
		output[(pos.y * 2 + 1) * (maze.size.x * 2 + 1) + pos.x * 2 + 1] = ' ';
	};

	const auto& setConn = [&](const Vec2& pos, const Vec2& connDir)
	{
		output[(pos.y * 2 + 1 + connDir.y) * (maze.size.x * 2 + 1) + pos.x * 2 + 1 + connDir.x] = ' ';
	};

	for (const auto& cell : maze.cells)
	{
		if (cell.visited)
		{
			set(cell.pos);

			if (cell.connEast) setConn(cell.pos, Vec2(1, 0));
			if (cell.connSouth) setConn(cell.pos, Vec2(0, 1));
		}
	}

	// Set goal/finish
	setConn(Vec2(0, 0), Vec2(-1, 0));
	setConn(Vec2(maze.size.x - 1, maze.size.y - 1), Vec2(1, 0));

	for (int y = 0; y < outputSize.y; y++)
	{
		for (int x = 0; x < outputSize.x; x++)
		{
			std::cout << output[y * outputSize.x + x];
		}

		std::cout << std::endl;
	}
}

int main()
{
	srand(time(nullptr));

	Maze maze(Vec2(21, 17));

	// Generate
	recurExtendPath(maze, Vec2(1, 1), Vec2(1, 1));

	// Output
	std::cout << "Comlud's Maze Gen 2" << std::endl << std::endl;
	print(maze);
}