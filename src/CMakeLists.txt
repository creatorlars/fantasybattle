cmake_minimum_required(VERSION 3.5.0)

set(SFML_STATIC_LIBRARIES TRUE)

set(SFML_DIR ${PROJECT_SOURCE_DIR}/external/sfml/lib/cmake/SFML)

find_package(SFML 2.6 COMPONENTS system window graphics network audio REQUIRED)

add_executable(simulator
	main.cpp
	application.cpp
	# ${PROJECT_SOURCE_DIR}/external/lzxemb/src/lzx/sfml/sfml_clock.cpp
	# ${PROJECT_SOURCE_DIR}/external/lzxemb/src/lzx/sfml/sfml_sleep.cpp
	# ${PROJECT_SOURCE_DIR}/external/lzxemb/src/lzx/sfml/sfml_render_window.cpp
	# ${PROJECT_SOURCE_DIR}/external/imgui/imgui_demo.cpp
	${PROJECT_SOURCE_DIR}/external/imgui/imgui_draw.cpp
	${PROJECT_SOURCE_DIR}/external/imgui/imgui_tables.cpp
	${PROJECT_SOURCE_DIR}/external/imgui/imgui_widgets.cpp
	# ${PROJECT_SOURCE_DIR}/external/imgui/imgui_knobs.cpp
	# ${PROJECT_SOURCE_DIR}/external/imgui/imgui_file_dialog.cpp
	${PROJECT_SOURCE_DIR}/external/imgui/imgui.cpp
	${PROJECT_SOURCE_DIR}/external/imgui-sfml/imgui-SFML.cpp
  )

target_include_directories(simulator
		PRIVATE
		${PROJECT_SOURCE_DIR}/src
		${PROJECT_SOURCE_DIR}/external/imgui
		${PROJECT_SOURCE_DIR}/external/imgui-sfml
		${PROJECT_SOURCE_DIR}/external/nlohmann-json/include
		# ${PROJECT_SOURCE_DIR}/external/lzxemb/include
		# ${PROJECT_SOURCE_DIR}/external/lzxemb/src/lzx/std
		# ${PROJECT_SOURCE_DIR}/external/lzxemb/src/lzx/sfml
		# ${PROJECT_SOURCE_DIR}/external/lzxemb/third_party/ArduinoJson
		${PROJECT_SOURCE_DIR}/external/etl/include
		# ${PROJECT_SOURCE_DIR}/library/software/ui
		${PROJECT_SOURCE_DIR}/external/sfml/include)

target_link_libraries(simulator sfml-system sfml-window sfml-graphics sfml-network sfml-audio)