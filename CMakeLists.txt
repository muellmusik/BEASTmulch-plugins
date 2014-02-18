#cmake -DSC_PATH=/code/git_SC/SuperCollider/ ..

cmake_minimum_required (VERSION 2.8)
project (BEASTmulchUGens)

set(CMAKE_MODULE_PATH ${SC_PATH}/cmake_modules
                      ${CMAKE_MODULE_PATH})
                      
#set(CMAKE_SOURCE_DIR ${SC_PATH})
  
                     
find_package(Sndfile)

#---------
# includes
#---------

if (SUPERNOVA)
    include_directories(${SC_PATH}/external_libraries/nova-tt)
    # actually just boost.atomic
    include_directories(${SC_PATH}/external_libraries/boost)
    include_directories(${SC_PATH}/external_libraries/boost_lockfree)
endif()

include_directories(${CMAKE_SOURCE_DIR}/include/)
include_directories(scfft_old/)

include_directories(${SC_PATH}/common)
include_directories(${SC_PATH}/plugin_interface)

include_directories(${SC_PATH}/include/plugin_interface)
include_directories(${SC_PATH}/include/common)
include_directories(${SC_PATH}/server/plugins) # for FFT_UGens.h
include_directories(${SC_PATH}/include/server) # for SC_UnitDef.h

# old-style directory layout
include_directories(${SC_PATH}/common/Headers/plugin_interface)
include_directories(${SC_PATH}/common/Headers/common)
include_directories(${SC_PATH}/common/Source/plugins) # for FFT_UGens.h

include_directories("${SC_PATH}/external_libraries/libsndfile/")
#include_directories(${SNDFILE_INCLUDE_DIR})


include_directories(plugin_headers/)

set(CMAKE_SHARED_MODULE_PREFIX "")
if(APPLE OR WIN32)
	set(CMAKE_SHARED_MODULE_SUFFIX ".scx")
endif()

add_library(BEASTMulchDelayUGens MODULE plugin_source/BEASTMulchDelayUGens.cpp)
add_library(BEASTMulchLoris MODULE plugin_source/BEASTMulchLoris.cpp)
add_library(BEASTMulchPV MODULE plugin_source/BEASTMulchPV.cpp)
add_library(BEASTMulchSendIndex MODULE plugin_source/BEASTMulchSendIndex.cpp)
add_library(BEASTMulchVBAP MODULE plugin_source/BEASTMulchVBAP.cpp)

target_link_libraries(BEASTMulchSendIndex ${SNDFILE_LIBRARIES})