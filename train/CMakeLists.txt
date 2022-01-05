cmake_minimum_required(VERSION 3.0)
project(train C)

set(CMAKE_C_STANDARD 99)

add_executable(train main.c)
add_executable(server server.c fileManager.c socketManager.c registrationManager.c signingManager.c switchManager.c)
add_executable(client client.c fileManager.c socketManager.c registrationManager.c signingManager.c switchManager.c)
target_link_libraries(server pthread)
target_link_libraries(client pthread)

#[[configure_file(contacts.txt <output> copyonly)]]
