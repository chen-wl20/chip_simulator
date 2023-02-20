#pragma once
#include<iostream>
#include "Trace_event.h"
#include<queue>

using namespace std;



// global
constexpr auto PORT_WIDTH = 256;

// memory
constexpr auto SRAM_DEPTH = 1000;

constexpr auto RAM_READ_LATENCY = 1;
constexpr auto RAM_WRITE_LATENCY = 1;
constexpr auto RAM_STATIC_POWER = 0.001;
constexpr auto RAM_READ_ENERGY = 0.1;
constexpr auto RAM_WRITE_ENERGY = 0.2;


constexpr auto DRAM_READ_LATENCY = 50;
constexpr auto DRAM_WRITE_LATENCY = 50;
constexpr auto DRAM_BURST_WRITE_LATENCY = 3;
constexpr auto DRAM_BURST_READ_LATENCY = 3;
constexpr auto DRAM_REFRESH_POWER = 0.1;
constexpr auto DRAM_READ_ENERGY = 10;
constexpr auto DRAM_WRITE_ENERGY = 20;
constexpr auto DRAM_TRANSFER_ENERGY = 50;
constexpr auto DRAM_LINE = 10;
constexpr auto DRAM_ENERGY_FACTOR = 0.9;


// control unit
constexpr auto ROM_START_PC = -1;