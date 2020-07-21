#pragma once
#include <array>
#include <mathematics/vector.h>
#include <vector>
#include "PerlinNoise.hpp"
#include <minelib/chunks/chunk.h>

namespace neko
{
	const static int mapSize = 256;
	const static int mapHeight = 16;
	const uint32_t seed = 0;
	const int undergroundHeight = 0;
	const int blocksNrBeforeStone = 4;

	const int plainsTerrainID = 0;
	const int hillsTerrainID = 1;
	const int mountainTerrainID = 2;

	const int greenBiomeID = 0;
	const int desertBiomeID = 1;
	const int snowBiomeID = 2;

	const float plainsfrequency = 0.1;
	const float frequency = 1;

	const int mountainOctaves = 3;
	const int octaves = 2;

	const int mountainHeightMultiplier = 64;
	const int heightMultiplier = 16;

	const Vec2i offset = Vec2i(0, 0);
	
	struct Zone
	{
	public:
		int zoneID;
		Zone* parentID;
		int terrain;
		int biome;
		Vec2i start;
		Vec2i end;
		std::vector<Zone*> XnegNeighboursIndexes;
		std::vector<Zone*> XposNeighboursIndexes;
		std::vector<Zone*> children;
		bool canSpawnMountain;
		Zone();
		Zone(int zoneID, Zone* parentID, int terrain, int biome, Vec2i start, Vec2i end);
		Zone(int zoneID, Zone* parentID, int terrain, int biome, Vec2i start, Vec2i end, bool canSpawnMountain);
	};
	
	class MapGeneration
	{
	public:

		void GenerateZoneSurface(Vec2i offset, int octaves, float frequency, Zone zone);
		std::array<std::array<std::array<int, kChunkSize>, kChunkSize>, kChunkSize> MapGeneration::GenerateMap3D(int posX, int posY);

		void GenerateZones(int bspCutIterations);
		void LerpZone(Zone zone, int lerpHeight);
		void CutZone(Zone* parentZone, int cutIteration, float maxCutPercentage, int mapSize);
		void LerpBlock(Vec2i startPos, int lerpHeight, bool yPos, bool yNeg, bool xPos, bool xNeg);
		bool CheckNeighbourhood(Zone zone1, Zone zone2);

		std::array<std::array<int, mapSize>, mapSize> heightMap;
		
	private:

	std::vector<Zone> allZones;
	std::vector<Zone*> finalZones;
	std::vector<int> possibleMountainZonesID;
	siv::PerlinNoise perlinNoise;
	
	int index = 0;
	int cutIteration = 4;
	float maxCutPercentage = 0.8;
	int lerpHeight = 4;
	//std::array<std::array<char, mapSize>, mapSize> map;
	};
}
