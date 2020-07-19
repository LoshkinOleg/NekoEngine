#pragma once
#include "minelib/map_generation.h"

namespace neko
{
	//Struct Zone
	Zone::Zone(int zoneID, int parentID, int terrain, int biome, Vec2i start, Vec2i end)
	{
		this->zoneID = zoneID;
		this->parentID = parentID;
		this->terrain = terrain;
		this->biome = biome;
		this->start = start;
		this->end = end;
		this->canSpawnMountain = false;
	}

	Zone::Zone(int zoneID, int parentID, int terrain, int biome, Vec2i start, Vec2i end, bool canSpawnMountain)
	{
		this->zoneID = zoneID;
		this->parentID = parentID;
		this->terrain = terrain;
		this->biome = biome;
		this->start = start;
		this->end = end;
		this->canSpawnMountain = canSpawnMountain;
	}

	//Class MapGeneration

	const std::array<std::array<int, mapSize>, mapSize> MapGeneration::GenerateMapSurface(Vec2i offset)
	{
		
	}

	std::array<std::array<std::array<int, mapHeight>, mapSize>, mapSize> MapGeneration::GenerateMap3D(std::array<std::array<int, mapSize>, mapSize> heightMap)
	{
		
	}

	void MapGeneration::CutZone(Zone parentZone, int cutIteration, float maxCutPercentage, int mapSize)
	{
		
	}

	void MapGeneration::LerpBlock(int map[][], Vec2i startPos, int lerpHeight, bool yPos, bool yNeg, bool xPos, bool xNeg)
	{
		
	}

	bool MapGeneration::CheckNeighbourhood(Zone zone1, Zone zone2)
	{
		
	}




}
