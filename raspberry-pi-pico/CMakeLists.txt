# Nécessite la version 3.13 au minimum pour fonctioner
cmake_minimum_required(VERSION 3.13)

# Inclue les imports de base du SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Déclare le projet en indiquant les langages de programmation utilisés
project(test_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise le SDK du Pico
pico_sdk_init()

# Inclue les fichiers sources du projet,
# chaque fichier doit être ajouté dans cette liste
add_executable(${PROJECT_NAME}
    led.hpp
    led.cpp
    bt.hpp
    bt.cpp
    passe_bande.h
    passe_bande.c
    main.cpp
)

# Permet de créer l'exécutable final
pico_add_extra_outputs(${PROJECT_NAME})

# Active / désactive la sortie standard par USB
pico_enable_stdio_usb(${PROJECT_NAME} 0)

# Active / désactive la sortie standard par UART
pico_enable_stdio_uart(${PROJECT_NAME} 1)

# Indique qu'il faut linker le projet avec la librairie standrad du Pico
target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    pico_multicore
    hardware_spi
    hardware_uart
    hardware_adc
)