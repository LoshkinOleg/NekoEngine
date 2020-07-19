#pragma once
#include "minelib/map_generation.h"
#include "PerlinNoise.hpp"

namespace neko
{
	//Struct Zone
	Zone::Zone()
	{
		
	}
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

	const std::array<std::array<int, mapSize>, mapSize> MapGeneration::GenerateZoneSurface(Vec2i offset, int octaves, float frequency, Zone zone)
	{
		perlinNoise = siv::PerlinNoise(seed);
		if (octaves <=0)
		{
			octaves = 1;
		}

		//int map[mapSize][mapSize];
		std::array<std::array<int, mapSize>, mapSize> map;

		for (int y = 0; y < mapSize; y++)
		{
			for (int x = 0; x < mapSize; x++)
			{
				float result = 0;
				for (int i = 0; i < octaves; i++)
				{
					float nx = (float)(x + offset.x) / (float)mapHeight - 0.5f, ny = (float)(y + offset.y) / (float)mapHeight - 0.5f;
					nx *= frequency;
					ny *= frequency;
					//nx += octaveOffsets[i].x;
					//ny += octaveOffsets[i].y;
					result += ((float)1 / (float)(i + 1) * ((perlinNoise.noise(nx,ny) + 1)/ 2));
				}
				map[y][x] = (int)(result * mapHeight);
			}
		}
		return map;
	}

	std::array<std::array<std::array<int, mapHeight>, mapSize>, mapSize> MapGeneration::GenerateMap3D(std::array<std::array<int, mapSize>, mapSize> heightMap)
	{
		std::array<std::array<std::array<int, mapHeight>, mapSize>, mapSize> map;
		for (int x = 0; x < mapSize; x++)
		{
			for (int z = 0; z < mapSize; z++)
			{
				for (int y = mapHeight - 1; y >= 0; y--)
				{
					//TODO CheckBiome
					// 0 = air
					// 1 = dirt
					// 2 = stone
					// 3 = grass
					// 4 = snow
					// 5 = sand
					int biomeSurfaceBlockID = 5;
					int blockBeforeStoneID = 5;
					if (heightMap[x][z] + undergroundHeight > y)
					{
						map[x][y][z] = 0;
					}
					else if (heightMap[x][z] + undergroundHeight == y)
					{
						map[x][y][z] = biomeSurfaceBlockID;
					}
					else if (heightMap[x][z] + undergroundHeight > y - blocksNrBeforeStone)
					{
						map[x][y][z] = blockBeforeStoneID;
					}
					else
					{
						map[x][y][z] = 2;
					}


				}
			}
		}
		return map;
	}

	void MapGeneration::GenerateZones(int mapSize, int bspCutIterations, int bspCutPercentage)
	{
		//Cut Map

		//Generate Mountain

		//Generate Noise

		//Generate Mountain Terrain

		//Find Mountain Neighbors to generate Hills

		//Generate Hills

		//Lerp between mountains and hills 

		//Find hills neightbours (that aren't hills or mountain)

		//Lerp between flatbiome and hills
	}

	
	void MapGeneration::CutZone(Zone parentZone, int cutIteration, float maxCutPercentage, int mapSize)
	{
		if (maxCutPercentage > 1)
		{
			maxCutPercentage = 1;
		}

		Zone childZone1;
		Zone childZone2;
		if (parentZone.end.x - parentZone.start.x > parentZone.end.y - parentZone.start.y)
		{
			//Cut in x
			int distance = parentZone.end.x - parentZone.start.x;
			int random = Random.Range((int)((distance / 2) - ((distance / 2) * maxCutPercentage)), (int)(((distance / 2) + ((distance / 2) * maxCutPercentage))));

			childZone1 = new Zone(index++, parentZone.parentID, 0, 0, parentZone.start, new Vec2i(parentZone.end.x - random, parentZone.end.y));
			childZone2 = new Zone(index++, parentZone.parentID, 0, 0, new Vec2i(parentZone.start.x + random, parentZone.start.y), parentZone.end);
		}
		else
		{
			//Cut in y
			int distance = parentZone.end.y - parentZone.start.y;
			int random = Random.Range((int)((distance / 2) - ((distance / 2) * maxCutPercentage)), (int)(((distance / 2) + ((distance / 2) * maxCutPercentage))));

			childZone1 = new Zone(index++, parentZone.parentID, 0, 0, parentZone.start, new Vec2i(parentZone.end.x, parentZone.end.y - random));
			childZone2 = new Zone(index++, parentZone.parentID, 0, 0, new Vec2i(parentZone.start.x, parentZone.start.y + random), parentZone.end);
		}
		//TODO Generate Neighbours ?
		allZones.push_back(childZone1);
		allZones.push_back(childZone2);
		if (cutIteration <= 0)
		{
			if (childZone1.start.x != 0 && childZone1.start.y >= mapSize - 1 && !(childZone1.end.x >= mapSize - 1) &&
				!(childZone1.end.y >= mapSize - 1))
			{
				childZone1.canSpawnMountain = true;
			}
			if (childZone2.start.x != 0 && childZone2.start.y >= mapSize - 1 && !(childZone2.end.x >= mapSize - 1) &&
				!(childZone2.end.y >= mapSize - 1))
			{
				childZone1.canSpawnMountain = true;
				mountainZonesIndexes.Add(childZone1.zoneID);
			}
			finalZones.push_back(childZone1);
			finalZones.push_back(childZone2);
		}
		else
		{
			CutZone(childZone1, cutIteration - 1, maxCutPercentage, mapSize);
			CutZone(childZone2, cutIteration - 1, maxCutPercentage, mapSize);
		}
	}

	void MapGeneration::LerpBlock(int map[][], Vec2i startPos, int lerpHeight, bool yPos, bool yNeg, bool xPos, bool xNeg)
	{
		//TODO check if value is higher

		if (yPos)
		{
			if (map[startPos.x][startPos.y] - lerpHeight > map[startPos.x][startPos.y + 1])
			{
				map[startPos.x][startPos.y + 1] = map[startPos.x][startPos.x] - lerpHeight;
				LerpBlock(map, Vec2i(startPos.x, startPos.y + 1), lerpHeight, yPos, yNeg, xPos, xNeg);
			}
		}

		if (yNeg)
		{
			if (map[startPos.x][startPos.y] - lerpHeight > map[startPos.x][startPos.y - 1])
			{
				map[startPos.x][startPos.y - 1] = map[startPos.x][startPos.x] - lerpHeight;
				LerpBlock(map, Vec2i(startPos.x, startPos.y - 1), lerpHeight, yPos, yNeg, xPos, xNeg);
			}

			if (xPos)
			{
				if (map[startPos.x][startPos.y] - lerpHeight > map[startPos.x + 1][startPos.y])
				{
					map[startPos.x + 1][startPos.y] = map[startPos.x][startPos.x] - lerpHeight;
					LerpBlock(map, Vec2i(startPos.x + 1, startPos.y), lerpHeight, yPos, yNeg, xPos, xNeg);
				}
			}

			if (xNeg)
			{
				if (map[startPos.x][startPos.y] - lerpHeight > map[startPos.x - 1][startPos.y])
				{
					map[startPos.x - 1][startPos.y] = map[startPos.x][startPos.x] - lerpHeight;
					LerpBlock(map, Vec2i(startPos.x - 1, startPos.y), lerpHeight, yPos, yNeg, xPos, xNeg);
				}
			}
		}
	}

	bool MapGeneration::CheckNeighbourhood(Zone zone1, Zone zone2)
	{
		if (zone1.end.x == zone2.start.x)
		{
			if (zone1.start.y > zone2.start.y)
			{
				if (zone1.end.y > zone2.start.y)
				{

				}
				else
				{
					return true;
				}
			}
			else
			{
				if (zone1.start.y > zone2.end.y)
				{
					return true;
				}
				else
				{

				}
			}
		}
		else if (zone1.end.y == zone2.start.y)
		{
			if (zone1.start.x > zone2.start.x)
			{
				if (zone1.start.x > zone2.end.x)
				{

				}
				else
				{
					return true;
				}
			}
			else
			{
				if (zone1.end.x > zone2.start.x)
				{
					return true;
				}
				else
				{

				}
			}
		}
		else if (zone2.end.x == zone1.start.x)
		{

		}
		else if (zone2.end.y == zone1.start.y)
		{

		}

		return false;
	}




}
