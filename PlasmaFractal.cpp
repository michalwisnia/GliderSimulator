//Original code by GitHub user Vesther https://github.com/Vesther/Cloud-Fractal

#include "plasmafractal.h"

PlasmaFractal::PlasmaFractal() {
	//printf("Started creating object\n");
	size = 513;
	range = 196;
	map[0][0] = rnd(0,255);
	map[0][size - 1] = rnd(0, 255);
	map[size - 1][0] = rnd(0, 255);
	map[size - 1][size - 1] = rnd(0,255);
	vert_count = 3 * 512 * 512 * 2; // 3 * (size-1)^2 * 2
	//printf("Map Corners\n");
	fractal();
	//printf("Fractal\n");
	clamp_map();
	//printf("Init Complete\n");
	transform_to_opengl();

}

int PlasmaFractal::rnd(int min, int max)
{
	return min + (rand() % static_cast<int>(max - min + 1));
}

void PlasmaFractal::diamond(int sideLength)
{
	int halfSide = sideLength / 2;

	for (int y = 0; y < size / (sideLength - 1); y++)
	{
		for (int x = 0; x < size / (sideLength - 1); x++)
		{
			int center_x = x * (sideLength - 1) + halfSide;
			int center_y = y * (sideLength - 1) + halfSide;

			int avg = (map[x * (sideLength - 1)][y * (sideLength - 1)] +
				map[x * (sideLength - 1)][(y + 1) * (sideLength - 1)] +
				map[(x + 1) * (sideLength - 1)][y * (sideLength - 1)] +
				map[(x + 1) * (sideLength - 1)][(y + 1) * (sideLength - 1)])
				/ 4.0f;

			map[center_x][center_y] = avg + rnd(-range, range);
		}
	}

}

void PlasmaFractal::average(int x, int y, int sideLength)
{
	float counter = 0;
	float accumulator = 0;

	int halfSide = sideLength / 2;

	if (x != 0)
	{
		counter += 1.0f;
		accumulator += map[y][x - halfSide];
	}
	if (y != 0)
	{
		counter += 1.0f;
		accumulator += map[y - halfSide][x];
	}
	if (x != size - 1)
	{
		counter += 1.0f;
		accumulator += map[y][x + halfSide];
	}
	if (y != size - 1)
	{
		counter += 1.0f;
		accumulator += map[y + halfSide][x];
	}

	map[y][x] = (accumulator / counter) + rnd(-range, range);
}

// Square step
void PlasmaFractal::square(int sideLength)
{
	int halfLength = sideLength / 2;

	for (int y = 0; y < size / (sideLength - 1); y++)
	{
		for (int x = 0; x < size / (sideLength - 1); x++)
		{
			// Top
			average(x * (sideLength - 1) + halfLength, y * (sideLength - 1), sideLength);
			// Right
			average((x + 1) * (sideLength - 1), y * (sideLength - 1) + halfLength, sideLength);
			// Bottom
			average(x * (sideLength - 1) + halfLength, (y + 1) * (sideLength - 1), sideLength);
			// Left
			average(x * (sideLength - 1), y * (sideLength - 1) + halfLength, sideLength);
		}
	}
}

// Main fractal generating loop
void PlasmaFractal::fractal()
{
	int sideLength = size / 2;

	diamond(size);
	square(size);

	range /= 2;

	while (sideLength >= 2)
	{
		diamond(sideLength + 1);
		square(sideLength + 1);
		sideLength /= 2;
		range /= 2;
	}
}

// Integer clamping helper
void PlasmaFractal::clamp(int* val, int min, int max)
{
	if (*val < min) *val = min;
	if (*val > max) *val = max;
}

// Function to clamp all map values
void PlasmaFractal::clamp_map()
{
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			clamp(&map[i][j], 0, 255);
		}
	}
}

// Debug print of the 2D array
void PlasmaFractal::print_map()
{
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			std::cout << std::setw(10) << map[i][j];
		}
		std::cout << std::endl;
	}
}

// File saving helper, saves to a PGM file (See https://en.wikipedia.org/wiki/Netpbm_format)
void PlasmaFractal::save_to_file()
{
	std::ofstream file_out("test.pgm", std::ios_base::out
		| std::ios_base::binary
		| std::ios_base::trunc
	);
	file_out << "P2" << "\n" << size << "\n" << size << "\n" << 255 << "\n";

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			file_out << map[i][j] << " ";
		}
		file_out << std::endl;
	}

	file_out.close();
}

void PlasmaFractal::transform_to_opengl() {
	int blockBegin = 0;
	printf("Began map creation\n");
	for (int x = 0; x < size; x++)
	{
		for (int y = 0; y < size; y++)
		{
			if (x < 512 && y < 512) {
				//printf("Wrote number to array %i %i at %i\n", x, y, blockBegin);
				verts[blockBegin + 0] = float(x - 256) / 300;
				verts[blockBegin + 1] = float(y - 256) / 300;
				verts[blockBegin + 2] = map[x][y] / 300;
				verts[blockBegin + 3] = 1.0f;
				verts[blockBegin + 4] = float(x - 256) / 300;
				verts[blockBegin + 5] = float(y - 256) / 300;
				verts[blockBegin + 6] = map[x][y] / 300;
				verts[blockBegin + 7] = 1.0f;
				verts[blockBegin + 8] = float(x - 256) / 300;
				verts[blockBegin + 9] = float(y - 256) / 300;
				verts[blockBegin + 10] = map[x][y] / 300;
				verts[blockBegin + 11] = 1.0f;
				blockBegin += 12;
			}
			if (x > 0 && y > 0) {
				//printf("Wrote number to array %i %i at %i\n", x, y, blockBegin);
				verts[blockBegin + 0] = float(x - 256) / 300;
				verts[blockBegin + 1] = float(y - 256) / 300;
				verts[blockBegin + 2] = map[x][y] / 300;
				verts[blockBegin + 3] = 1.0f;
				verts[blockBegin + 4] = float(x - 256) / 300;
				verts[blockBegin + 5] = float(y - 256) / 300;
				verts[blockBegin + 6] = map[x][y] / 300;
				verts[blockBegin + 7] = 1.0f;
				verts[blockBegin + 8] = float(x - 256) / 300;
				verts[blockBegin + 9] = float(y - 256) / 300;
				verts[blockBegin + 10] = map[x][y] / 300;
				verts[blockBegin + 11] = 1.0f;
				blockBegin += 12;
			}
		}
	}
	for (int i = 0; i < sizeof(colors) / sizeof(float); i++) {
		colors[i] = 1.0f;
	}
	for (int i = 0; i < sizeof(normals) / sizeof(float); i++) {
		if (i % 4 == 2) {
			normals[i] = 1.f;
		}
		else {
			normals[i] = 0.f;
		}
	}

	printf("Size of verts = %i and vert count = %i\n", sizeof(verts) / sizeof(float), vert_count);
	printf("Finished map creation\n");
}

void PlasmaFractal::draw_solid() {
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(0, 4, GL_FLOAT, false, 0, verts);
	glVertexAttribPointer(1, 4, GL_FLOAT, false, 0, normals);
	glVertexAttribPointer(3, 4, GL_FLOAT, false, 0, colors);


	glDrawArrays(GL_TRIANGLES, 0, vert_count);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(3);
}