file(GLOB_RECURSE PRIVATE_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_library(JamClientAutoCode STATIC
    ${PRIVATE_SOURCES}
)

target_include_directories(JamClientAutoCode
    PUBLIC
        ${CMAKE_SOURCE_DIR}
)

