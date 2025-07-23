target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(${PROJECT_NAME} PUBLIC
    default_command.c
    "build_command.c"
    init_command.c
    list_command.c
    help_command.c
    note_matching_command.c
)