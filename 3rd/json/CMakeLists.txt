project(json)

set(JSON_SRCS
	json_reader.cpp
	json_value.cpp
	json_writer.cpp
)

ADD_LIBRARY(json STATIC ${JSON_SRCS})
SET_TARGET_PROPERTIES(json PROPERTIES OUTPUT_NAME "json")

