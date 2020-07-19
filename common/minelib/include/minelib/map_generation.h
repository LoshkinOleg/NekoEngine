#pragma once
#include <array>
#include <mathematics/vector.h>
#include <vector>
#include "PerlinNoise.hpp"

namespace neko
{
	const int mapSize = 1024;
	const int mapHeight = 128;
	const uint32_t seed = 0;
	const int undergroundHeight = 16;
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

		const std::array<std::array<int, mapSize>, mapSize> GenerateZoneSurface(Vec2i offset, int octaves, float frequency, Zone zone);
		std::array<std::array<std::array<int, mapHeight>, mapSize>, mapSize> GenerateMap3D(std::array<std::array<int, mapSize>, mapSize> heightMap);

		void GenerateZones(int mapSize, int bspCutIterations, int bspCutPercentage);
		void CutZone(Zone parentZone, int cutIteration, float maxCutPercentage, int mapSize);
		void LerpBlock(int map[][], Vec2i startPos, int lerpHeight, bool yPos, bool yNeg, bool xPos, bool xNeg);
		bool CheckNeighbourhood(Zone zone1, Zone zone2);
		
	private:

	std::vector<Zone> allZones;
	std::vector<Zone> finalZones;
	siv::PerlinNoise perlinNoise;
	
		
	int index = 0;
		
	};
}
