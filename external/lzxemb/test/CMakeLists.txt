cmake_minimum_required(VERSION 3.5.0)
project(lzxemb_unit_tests LANGUAGES CXX)

add_executable(lzxemb_test_platform_sleep
	test_platform_sleep.cpp
	${PROJECT_SOURCE_DIR}/../src/lzx/std/clock.cpp
	${PROJECT_SOURCE_DIR}/../src/lzx/std/sleep.cpp
  )

target_include_directories(lzxemb_test_platform_sleep
		PRIVATE
		${PROJECT_SOURCE_DIR}/
		${PROJECT_SOURCE_DIR}/../src/lzx/std
		${PROJECT_SOURCE_DIR}/../include 
		${PROJECT_SOURCE_DIR}/../third_party/etl/include)
		
add_test(lzxemb_unit_test_platform_sleep lzxemb_test_platform_sleep)


add_executable(lzxemb_test_synthesis_parameter
	test_synthesis_parameter.cpp
	${PROJECT_SOURCE_DIR}/../src/lzx/std/clock.cpp
	${PROJECT_SOURCE_DIR}/../src/lzx/std/sleep.cpp
  )

target_include_directories(lzxemb_test_synthesis_parameter
		PRIVATE
		${PROJECT_SOURCE_DIR}/
		${PROJECT_SOURCE_DIR}/../src/lzx/std
		${PROJECT_SOURCE_DIR}/../include 
		${PROJECT_SOURCE_DIR}/../third_party/etl/include)
		
add_test(lzxemb_unit_test_synthesis_parameter lzxemb_test_synthesis_parameter)
