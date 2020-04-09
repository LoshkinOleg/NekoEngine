#include <benchmark/benchmark.h>
#include <random>
#include <algorithm>
#include <numeric>
#include <vector>
#include <mathematics/vector.h>
#include "engine/custom_allocator.h"
#include "custom_container/string.h"

const long fromRange = 8;
const long toRange = 1 << 15;
const float start = -100.0f;
const float end = 100.0f;


static void BM_StringWithAddedAllocator(benchmark::State& state)
{
    size_t n = state.range(0);
    size_t memorySize = 1024;
    void* data = malloc(memorySize);
    neko::FreeListAllocator allocator = neko::FreeListAllocator(memorySize, data);

    neko::String s1 = neko::String(allocator, "AA");
    neko::String s2 = neko::String(allocator, "BB");


    for (auto _ : state)
    {
        for (int i = 0; i < n; i++) {
            s2 = "BB";
            benchmark::DoNotOptimize(s2 +=s1);
        }
    }
}

BENCHMARK(BM_StringWithAddedAllocator)->Range(fromRange, toRange);

static void BM_StringSTD(benchmark::State& state)
{
    size_t n = state.range(0);
    std::string s1 = "A";
    std::string s2 = "B";

    for (auto _ : state)
    {
        for (int i = 0; i < n; i++) {
            s2 = "B";
            benchmark::DoNotOptimize(s2 += s1);
        }
    }
}

BENCHMARK(BM_StringSTD)->Range(fromRange, toRange);


BENCHMARK_MAIN();