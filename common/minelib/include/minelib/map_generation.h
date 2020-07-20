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
	
	struct Zone
	{
	public:
		int zoneID;
		int parentID;
		int terrain;
		int biome;
		Vec2i start;
		Vec2i end;
		std::vector<int> neighboursIndexes;
		bool canSpawnMountain;
		Zone();
		Zone(int zoneID, int parentID, int terrain, int biome, Vec2i start, Vec2i end);
		Zone(int zoneID, int parentID, int terrain, int biome, Vec2i start, Vec2i end, bool canSpawnMountain);
	};
	
	class MapGeneration
	{
	public:

		void GenerateZoneSurface(Vec2i offset, int octaves, float frequency, Zone zone);
		std::array<std::array<std::array<int, kChunkSize>, kChunkSize>, kChunkSize> MapGeneration::GenerateMap3D(int posX, int posY);

		void GenerateZones(int mapSize, int bspCutIterations, int bspCutPercentage);
		void CutZone(Zone parentZone, int cutIteration, float maxCutPercentage, int mapSize);
		void LerpBlock(std::array<std::array<int, mapSize>, mapSize> map, Vec2i startPos, int lerpHeight, bool yPos, bool yNeg, bool xPos, bool xNeg);
		bool CheckNeighbourhood(Zone zone1, Zone zone2);

		std::array<std::array<int, mapSize>, mapSize> heightMap;
		
	private:

	std::vector<Zone> allZones;
	std::vector<Zone> finalZones;
	siv::PerlinNoise perlinNoise;
	
	int index = 0;
	//std::array<std::array<char, mapSize>, mapSize> map;
	};
}
