cmake_minimum_required(VERSION 3.10)
project(Asteroids)

set(CMAKE_CXX_STANDARD 11)

configure_file(images/background.jpg images/background.jpg COPYONLY)
configure_file(images/fire_blue.png images/fire_blue.png COPYONLY)
configure_file(images/fire_red.png images/fire_red.png COPYONLY)
configure_file(images/rock.png images/rock.png COPYONLY)
configure_file(images/rock_small.png images/rock_small.png COPYONLY)
configure_file(images/spaceship.png images/spaceship.png COPYONLY)
configure_file(images/explosions/type_B.png images/explosions/type_B.png COPYONLY)
configure_file(images/explosions/type_C.png images/explosions/type_C.png COPYONLY)
configure_file(images/UFO.png images/UFO.png COPYONLY)
configure_file(images/lifeicon.png images/lifeicon.png COPYONLY)
configure_file(images/finallevelback.jpg images/finallevelback.jpg COPYONLY)

configure_file(sounds/StarCommander1.flac sounds/StarCommander1.flac COPYONLY)
configure_file(sounds/robotglitch.ogg sounds/robotglitch.ogg COPYONLY)
configure_file(sounds/ufosound.ogg sounds/ufosound.ogg COPYONLY)
configure_file(sounds/JustDanceLadyGaGa.flac sounds/JustDanceLadyGaGa.flac COPYONLY)

configure_file(fonts/VideoPhreak.ttf fonts/VideoPhreak.ttf COPYONLY)

#file(COPY images DESTINATION images)

# Define sources and executable
set(EXECUTABLE_NAME "Asteroids")

add_executable(${EXECUTABLE_NAME} main.cpp)

# Detect and add SFML
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
#Find any version 2.X of SFML
#See the FindSFML.cmake file for additional details and instructions
find_package(SFML 2 REQUIRED network audio graphics window system)
if(SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
endif()