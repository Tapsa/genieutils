# Available settings:
#
# GUTILS_TOOLS:BOOL     if true to enable compilation of gutils tools
# GUTILS_TEST:BOOL      if true some debug/test classes will be compiled
# STATIC_COMPILE:BOOL   if true we statically compile the library

cmake_minimum_required(VERSION 3.11)

set(Genieutils_LIBRARY genieutils)

project(genieutils)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake/)
#set(CMAKE_CXX_FLAGS "-std=gnu++0x")

if(STATIC_COMPILE)
  set(Boost_USE_STATIC_LIBS        ON)
  #set(Boost_USE_STATIC_RUNTIME     ON)
endif(STATIC_COMPILE)

set(Boost_USE_MULTITHREADED      ON)

# dependencies:

find_package(ZLIB REQUIRED)
find_package(LZ4 REQUIRED)
find_package(Boost 1.55 COMPONENTS iostreams REQUIRED)

if(GUTILS_TOOLS)
  find_package(Boost 1.55 COMPONENTS program_options REQUIRED)
endif(GUTILS_TOOLS)

if(GUTILS_TEST)
  find_package(Boost 1.55 COMPONENTS unit_test_framework program_options REQUIRED)
  find_package(SFML COMPONENTS SYSTEM WINDOW GRAPHICS REQUIRED)
endif(GUTILS_TEST)

find_package(Iconv REQUIRED)

if(${ICONV_FOUND})
  message(STATUS "Iconv found! Language file support: ENABLED")
  set(GU_LANG_SUPPORT true)
else(${ICONV_FOUND})
  message(STATUS "Iconv not found: Language file support: DISABLED")
  set(GU_LANG_SUPPORT false)
endif(${ICONV_FOUND})

# include directories:

set(GU_INCLUDE_DIRS include/ ../ ${Boost_INCLUDE_DIRS})

if(${ICONV_FOUND})
  set(GU_INCLUDE_DIRS ${GU_INCLUDE_DIRS} ${ICONV_INCLUDE_DIR})
endif(${ICONV_FOUND})

if(${GUTILS_TEST})
  set(GU_INCLUDE_DIRS ${GU_INCLUDE_DIRS} ${SFML_INCLUDE_DIR})
endif(${GUTILS_TEST})

include_directories(${GU_INCLUDE_DIRS})

add_definitions(-Wall -Wsign-compare)

#------------------------------------------------------------------------------#
# Source files: 
#------------------------------------------------------------------------------#
set(FILE_SRC
    src/file/ISerializable.cpp
    src/file/IFile.cpp
    src/file/Compressor.cpp
    )

if(${GU_LANG_SUPPORT})
  set(PCRIO_SRC ../pcrio/pcrio.c)
  set_source_files_properties(../pcrio/pcrio.c PROPERTIES LANGUAGE CXX)

  set(LANG_SRC ${PCRIO_SRC}
      src/lang/LangFile.cpp
     )
else(${GU_LANG_SUPPORT})
  set(LANG_SRC)
endif(${GU_LANG_SUPPORT})
    

set(DAT_SRC
    src/dat/Research.cpp 
   
    src/dat/Civ.cpp
    src/dat/TechageEffect.cpp
    src/dat/Techage.cpp

    src/dat/TechTree.cpp
    src/dat/TerrainCommon.cpp
    src/dat/Terrain.cpp
    src/dat/TerrainBlock.cpp
    src/dat/TerrainBorder.cpp
    src/dat/GraphicAttackSound.cpp
    src/dat/GraphicDelta.cpp
    src/dat/Graphic.cpp
    src/dat/SoundItem.cpp
    src/dat/Sound.cpp
    src/dat/PlayerColour.cpp
    src/dat/DatFile.cpp
    src/dat/TerrainPassGraphic.cpp
    src/dat/TerrainRestriction.cpp

    src/dat/Unit.cpp
    src/dat/UnitCommand.cpp
    src/dat/UnitHeader.cpp
    src/dat/UnitLine.cpp
    src/dat/RandomMap.cpp

    src/dat/unit/AttackOrArmor.cpp
    src/dat/unit/DamageGraphic.cpp
    src/dat/unit/DeadFish.cpp 
    src/dat/unit/Bird.cpp
    src/dat/unit/Type50.cpp
    src/dat/unit/Projectile.cpp
    src/dat/unit/Creatable.cpp
    src/dat/unit/Building.cpp
    )

set(RESOURCE_SRC
    src/resource/PalFile.cpp
    src/resource/SlpFile.cpp
    src/resource/SlpFrame.cpp
    src/resource/SmpFile.cpp
    src/resource/SmpFrame.cpp
    src/resource/SmxFile.cpp
    src/resource/SmxFrame.cpp
    src/resource/DrsFile.cpp
    src/resource/Color.cpp
    src/resource/BinaFile.cpp
    )

set(SCRIPT_SRC
    src/script/ScnFile.cpp
    src/script/scn/ScnResource.cpp
    src/script/scn/ScnPlayerData.cpp
    src/script/scn/MapDescription.cpp
    src/script/scn/Trigger.cpp
    )

set(UTIL_SRC
    src/util/Logger.cpp
    )

# Tool sources:

set(TEST_SRC
    src/tools/test/test.cpp
    src/tools/bincompare/bincomp.cpp
   )

set(EXTRACT_SRC src/tools/extract/datextract.cpp)

set(BINCOMP_SRC src/tools/bincompare/bincomp.cpp
                src/tools/bincompare/main.cpp)
                


#------------------------------------------------------------------------------#
# Executable:
#------------------------------------------------------------------------------#

if(STATIC_COMPILE)
  add_library(${Genieutils_LIBRARY} STATIC ${FILE_SRC} ${LANG_SRC} ${DAT_SRC} 
                                    ${RESOURCE_SRC} ${UTIL_SRC} ${SCRIPT_SRC} )
  target_link_libraries(${Genieutils_LIBRARY} ${Boost_LIBRARIES} ${ZLIB_LIBRARIES} ${ICONV_LIBRARIES})
else()
  add_library(${Genieutils_LIBRARY} SHARED ${FILE_SRC} ${LANG_SRC} ${DAT_SRC} 
                                    ${RESOURCE_SRC} ${UTIL_SRC} ${SCRIPT_SRC} )
  target_link_libraries(${Genieutils_LIBRARY} ${ZLIB_LIBRARIES} ${LZ4_LIBRARIES} ${Boost_LIBRARIES} ${ICONV_LIBRARIES})
endif(STATIC_COMPILE)

#add_executable(main main.cpp)
#target_link_libraries(main ${Genieutils_LIBRARY})

#------------------------------------------------------------------------------#
# Dev tools:
#------------------------------------------------------------------------------#

 # test doesn't build :(
 # add_executable(test ${TEST_SRC})
 # target_link_libraries(test ${Genieutils_LIBRARY} ${Boost_LIBRARIES} ) #${ZLIB_LIBRARIES} ${Boost_LIBRARIES} ${Genieutils_LIBRARY})

if(GUTILS_TOOLS)
  add_executable(datextract ${EXTRACT_SRC})
  target_link_libraries(datextract ${ZLIB_LIBRARIES} ${Boost_LIBRARIES} ${Genieutils_LIBRARY})

  add_executable(bincomp ${BINCOMP_SRC})
endif(GUTILS_TOOLS)

#------------------------------------------------------------------------------#
# Debug/Test:
#------------------------------------------------------------------------------#
if(GUTILS_TEST)
  add_executable(main main.cpp)
  target_link_libraries(main ${Genieutils_LIBRARY}
                      ${SFML_WINDOW_LIBRARY}
                      ${SFML_SYSTEM_LIBRARY}
                      ${SFML_GRAPHICS_LIBRARY}
  )
endif(GUTILS_TEST)
