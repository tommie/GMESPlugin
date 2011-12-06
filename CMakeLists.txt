cmake_minimum_required(VERSION 2.8.2)

project(gmesplugin C CXX)
include_directories(${CMAKE_CURRENT_LIST_DIR}/gme)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

set(GME_SOURCES
	gme/gme/Ay_Apu.cpp
	gme/gme/Ay_Cpu.cpp
	gme/gme/Ay_Emu.cpp
	gme/gme/Blip_Buffer.cpp
	gme/gme/Classic_Emu.cpp
	gme/gme/Data_Reader.cpp
	gme/gme/Dual_Resampler.cpp
	gme/gme/Effects_Buffer.cpp
	gme/gme/Fir_Resampler.cpp
	gme/gme/Gb_Apu.cpp
	gme/gme/Gb_Cpu.cpp
	gme/gme/Gb_Oscs.cpp
	gme/gme/Gbs_Emu.cpp
	gme/gme/gme.cpp
	gme/gme/Gme_File.cpp
	gme/gme/Gym_Emu.cpp
	gme/gme/Hes_Apu.cpp
	gme/gme/Hes_Cpu.cpp
	gme/gme/Hes_Emu.cpp
	gme/gme/Kss_Cpu.cpp
	gme/gme/Kss_Emu.cpp
	gme/gme/Kss_Scc_Apu.cpp
	gme/gme/M3u_Playlist.cpp
	gme/gme/Multi_Buffer.cpp
	gme/gme/Music_Emu.cpp
	gme/gme/Nes_Apu.cpp
	gme/gme/Nes_Cpu.cpp
	gme/gme/Nes_Fme7_Apu.cpp
	gme/gme/Nes_Namco_Apu.cpp
	gme/gme/Nes_Oscs.cpp
	gme/gme/Nes_Vrc6_Apu.cpp
	gme/gme/Nsfe_Emu.cpp
	gme/gme/Nsf_Emu.cpp
	gme/gme/Sap_Apu.cpp
	gme/gme/Sap_Cpu.cpp
	gme/gme/Sap_Emu.cpp
	gme/gme/Sms_Apu.cpp
	gme/gme/Snes_Spc.cpp
	gme/gme/Spc_Cpu.cpp
	gme/gme/Spc_Dsp.cpp
	gme/gme/Spc_Emu.cpp
	gme/gme/Vgm_Emu.cpp
	gme/gme/Vgm_Emu_Impl.cpp
	gme/gme/Ym2413_Emu.cpp
	gme/gme/Ym2612_Emu.cpp)

set(PLUGIN_SOURCES
	gme_parser.c
	gme_player.c
	GMEsplugin.c)

add_library(gmesplugin MODULE
	${GME_SOURCES}
	${PLUGIN_SOURCES})

set_target_properties(gmesplugin PROPERTIES
	PREFIX ""
	SUFFIX ".splugin"
	COMPILE_FLAGS "-O3 -Wall"
	LINK_FLAGS "-Wl,--no-undefined")

set_source_files_properties(${PLUGIN_SOURCES} PROPERTIES
	COMPILE_FLAGS "-std=gnu99")
