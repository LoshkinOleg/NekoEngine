#pragma once
#include "minelib/map_generation.h"
#include "PerlinNoise.hpp"
#include <time.h>

namespace neko
{
	//Struct Zone
	Zone::Zone()
	{
		
	}
	Zone::Zone(int zoneID, Zone* parentID, int terrain, int biome, Vec2i start, Vec2i end)
	{
		this->zoneID = zoneID;
		this->parentID = parentID;
		this->terrain = terrain;
		this->biome = biome;
		this->start = start;
		this->end = end;
		this->canSpawnMountain = false;
	}

	Zone::Zone(int zoneID, Zone* parentID, int terrain, int biome, Vec2i start, Vec2i end, bool canSpawnMountain)
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

	void MapGeneration::GenerateZoneSurface(Vec2i offset, int octaves, float frequency, Zone zone)
	{
		if (octaves <= 0)
		{
			octaves = 1;
		}
		perlinNoise = siv::PerlinNoise(seed);
		

		//int map[mapSize][mapSize];
		//std::array<std::array<int, mapSize>, mapSize> heightMap;

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
				heightMap[y][x] = (int)(result * mapHeight);
			}
		}
		//return map;
	}

	std::array<std::array<std::array<int, kChunkSize>, kChunkSize>, kChunkSize> MapGeneration::GenerateMap3D(int posX, int posY)
	{
		std::array<std::array<std::array<int, kChunkSize>, kChunkSize>, kChunkSize> map;
		if(posX < 0 || posY < 0 || posX >= mapSize|| posY >= mapSize)
		{
			for (int x = 0; x < kChunkSize; x++)
			{
				for (int z = 0; z < kChunkSize; z++)
				{
					for (int y = kChunkSize - 1; y >= 0; y--)
					{
						map[x][y][z] = 0;


					}
				}
			}
			
		}
		else
		{
			for (int x = 0; x < kChunkSize; x++)
			{
				for (int z = 0; z < kChunkSize; z++)
				{
					for (int y = kChunkSize - 1; y >= 0; y--)
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
						if (heightMap[x + posX][z + posY] + undergroundHeight > y)
						{
							map[x][y][z] = 0;
						}
						else if (heightMap[x + posX][z + posY] + undergroundHeight == y)
						{
							map[x][y][z] = biomeSurfaceBlockID;
						}
						else if (heightMap[x + posX][z + posY] + undergroundHeight > y - blocksNrBeforeStone)
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
		}
		return map;
	}

	void MapGeneration::GenerateZones(int bspCutIterations)
	{
		srand(seed);
		//Cut Map
		Zone mapZone = Zone(index++, nullptr, 0, 0, Vec2i(0, 0), Vec2i(mapSize, mapSize));
		allZones.push_back(mapZone);
		CutZone(&mapZone, cutIteration, maxCutPercentage, mapSize);
		
		//Generate Mountain
		int randomMountain;
		randomMountain = rand() % possibleMountainZonesID.size();
		
		allZones[randomMountain].zoneID = mountainTerrainID;
		allZones[randomMountain].biome = snowBiomeID;
		int mountainID = -1;

		for (Zone* element : allZones[randomMountain].XposNeighboursIndexes)
		{
			element->terrain = hillsTerrainID;
			element->biome = desertBiomeID;
		}
		for (Zone* element : allZones[randomMountain].XnegNeighboursIndexes)
		{
			element->terrain = hillsTerrainID;
		}

		
		
		
		//Generate Noise

		for (Zone* element : finalZones)
		{
			switch (element->terrain)
			{
			//plains
			case 0:
				GenerateZoneSurface(offset, octaves, plainsfrequency, *element);
				break;
			//hills
			case 1:
				GenerateZoneSurface(offset, octaves, frequency, *element);
				break;
			//mountains
			case 2:
				GenerateZoneSurface(offset, mountainOctaves, frequency, *element);
				break;
			}
		}
		//Lerp between mountains and hills
		LerpZone(*finalZones[mountainID], lerpHeight);
		
		for (Zone* element : finalZones[mountainID]->XposNeighboursIndexes)
		{
			LerpZone(*element, lerpHeight);
		}
		for (Zone* element : finalZones[mountainID]->XnegNeighboursIndexes)
		{
			LerpZone(*element, lerpHeight);
		}
	}

	
	void MapGeneration::CutZone(Zone* parentZone, int cutIteration, float maxCutPercentage, int mapSize)
	{
		srand(seed);
		if (maxCutPercentage > 1)
		{
			maxCutPercentage = 1;
		}
		else if(maxCutPercentage < 0)
		{
			maxCutPercentage = 0;
		}

		Zone childZone1;
		Zone childZone2;
		if (parentZone->end.x - parentZone->start.x > parentZone->end.y - parentZone->start.y)
		{
			//Cut in x
			int distance = parentZone->end.x - parentZone->start.x;
			int random = (rand() % (int)(distance * maxCutPercentage)) + (distance * maxCutPercentage)/2;
			
			childZone1 = Zone(index++, parentZone, 0, 0, parentZone->start, Vec2i(parentZone->end.x - random, parentZone->end.y));
			childZone2 = Zone(index++, parentZone, 0, 0, Vec2i(parentZone->start.x + random, parentZone->start.y), parentZone->end);
		}
		else
		{
			//Cut in y
			int distance = parentZone->end.y - parentZone->start.y;
			int random = (rand() % (int)(distance * maxCutPercentage)) + (distance * maxCutPercentage) / 2;

			childZone1 = Zone(index++, parentZone, 0, 0, parentZone->start, Vec2i(parentZone->end.x, parentZone->end.y - random));
			childZone2 = Zone(index++, parentZone, 0, 0, Vec2i(parentZone->start.x, parentZone->start.y + random), parentZone->end);
		}
		allZones.push_back(childZone1);
		allZones.push_back(childZone2);

		//Check Neighbourhood
		for (Zone* element : allZones[allZones.size() -1].parentID->XposNeighboursIndexes)
		{
			for (Zone* element : element->children)
			{
				if(CheckNeighbourhood(allZones[allZones.size() - 1], *element))
				{
					if (allZones[allZones.size() - 1].start.x > element->start.x)
					{

						allZones[allZones.size() - 1].XnegNeighboursIndexes;
					}
					else
					{
						allZones[allZones.size() - 1].XposNeighboursIndexes;
					}
				}
			}
		}
		for (Zone* element : allZones[allZones.size() - 1].parentID->XnegNeighboursIndexes)
		{
			for (Zone* element : element->children)
			{
				if (CheckNeighbourhood(allZones[allZones.size() - 1], *element))
				{
					if (allZones[allZones.size() - 1].start.x > element->start.x)
					{

						allZones[allZones.size() - 1].XnegNeighboursIndexes;
					}
					else
					{
						allZones[allZones.size() - 1].XposNeighboursIndexes;
					}
				}
			}
		}
		for (Zone* element : allZones[allZones.size() - 2].parentID->XposNeighboursIndexes)
		{
			for (Zone* element : element->children)
			{
				if (CheckNeighbourhood(allZones[allZones.size() - 2], *element))
				{
					if (allZones[allZones.size() - 2].start.x > element->start.x)
					{

						allZones[allZones.size() - 2].XnegNeighboursIndexes;
					}
					else
					{
						allZones[allZones.size() - 2].XposNeighboursIndexes;
					}
				}
			}
		}
		for (Zone* element : allZones[allZones.size() - 2].parentID->XnegNeighboursIndexes)
		{
			for (Zone* element : element->children)
			{
				if (CheckNeighbourhood(allZones[allZones.size() - 2], *element))
				{
					if (allZones[allZones.size() - 2].start.x > element->start.x)
					{

						allZones[allZones.size() - 2].XnegNeighboursIndexes;
					}
					else
					{
						allZones[allZones.size() - 2].XposNeighboursIndexes;
					}
				}
			}
		}
		
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
				//mountainZonesIndexes.Add(childZone1.zoneID);
			}
			finalZones.push_back(&childZone1);
			finalZones.push_back(&childZone2);
		}
		else
		{
			CutZone(&childZone1, cutIteration - 1, maxCutPercentage, mapSize);
			CutZone(&childZone2, cutIteration - 1, maxCutPercentage, mapSize);
		}
	}

	void MapGeneration::LerpZone(Zone zone, int lerpHeight)
	{
		LerpBlock(zone.start, lerpHeight, false, true, false, true);
		LerpBlock(zone.end, lerpHeight, true, false, true, false);
		LerpBlock(Vec2i(zone.start.x, zone.end.y), lerpHeight, true, false, false, true);
		LerpBlock(Vec2i(zone.start.y, zone.end.x), lerpHeight, false, true, true, false);

		for(int i = zone.start.x + 1; i < zone.end.x; i++)
		{
			LerpBlock(Vec2i(i, zone.start.y), lerpHeight, false, true, false, false);
			LerpBlock(Vec2i(i, zone.end.y), lerpHeight, true, false, false, false);
		}
		for (int i = zone.start.y + 1; i < zone.end.y; i++)
		{
			LerpBlock(Vec2i(zone.start.x, i), lerpHeight, false, false, false, true);
			LerpBlock(Vec2i(zone.end.y, i), lerpHeight, false, false, true, false);
		}
		
	}


	void MapGeneration::LerpBlock(Vec2i startPos, int lerpHeight, bool yPos, bool yNeg, bool xPos, bool xNeg)
	{
		//TODO check if value is higher

		if (yPos)
		{
			if (heightMap[startPos.x][startPos.y] - lerpHeight > heightMap[startPos.x][startPos.y + 1])
			{
				heightMap[startPos.x][startPos.y + 1] = heightMap[startPos.x][startPos.x] - lerpHeight;
				LerpBlock(Vec2i(startPos.x, startPos.y + 1), lerpHeight, yPos, yNeg, xPos, xNeg);
			}
		}

		if (yNeg)
		{
			if (heightMap[startPos.x][startPos.y] - lerpHeight > heightMap[startPos.x][startPos.y - 1])
			{
				heightMap[startPos.x][startPos.y - 1] = heightMap[startPos.x][startPos.x] - lerpHeight;
				LerpBlock(Vec2i(startPos.x, startPos.y - 1), lerpHeight, yPos, yNeg, xPos, xNeg);
			}

			if (xPos)
			{
				if (heightMap[startPos.x][startPos.y] - lerpHeight > heightMap[startPos.x + 1][startPos.y])
				{
					heightMap[startPos.x + 1][startPos.y] = heightMap[startPos.x][startPos.x] - lerpHeight;
					LerpBlock(Vec2i(startPos.x + 1, startPos.y), lerpHeight, yPos, yNeg, xPos, xNeg);
				}
			}

			if (xNeg)
			{
				if (heightMap[startPos.x][startPos.y] - lerpHeight > heightMap[startPos.x - 1][startPos.y])
				{
					heightMap[startPos.x - 1][startPos.y] = heightMap[startPos.x][startPos.x] - lerpHeight;
					LerpBlock(Vec2i(startPos.x - 1, startPos.y), lerpHeight, yPos, yNeg, xPos, xNeg);
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
			}
		}
		else if (zone2.end.x == zone1.start.x)
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
			}
		}
		else if (zone2.end.y == zone1.start.y)
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
			}
		}

		return false;
	}




}
