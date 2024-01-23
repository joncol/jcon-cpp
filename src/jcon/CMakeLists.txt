project(jcon)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB ${PROJECT_NAME}_headers *.h)
file(GLOB ${PROJECT_NAME}_sources *.cpp)

add_definitions(-DJCON_DLL)

add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_headers} ${${PROJECT_NAME}_sources})
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${${PROJECT_NAME}_headers}")
find_package(Qt6 REQUIRED COMPONENTS Core Network WebSockets)
qt_standard_project_setup()
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Network)

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include/jcon
        ARCHIVE DESTINATION lib/static)
