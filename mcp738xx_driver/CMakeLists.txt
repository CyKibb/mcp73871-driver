find_path(CHECK_INCLUDE_DIR check.h)
find_library(CHECK_LIBRARY NAMES check)

add_executable(mcp738xx_driver_tests ./tests/mcp738xx_driver_tests.c)
target_include_directories(mcp738xx_driver_tests PUBLIC ${CHECK_INCLUDE_DIR} includes)
target_link_libraries(mcp738xx_driver_tests ${CHECK_LIBRARY} mcp738xx_driver)

add_library(mcp738xx_driver ./src/mcp_bms.c)

target_include_directories(mcp738xx_driver PUBLIC includes)