#cборка iceBw
message ("Построение вспомогательных программ")

project(util) #имя проекта


# Установка переменной со списком исходников
set(SOURSE_BIN iceb_print.cpp iceb_print_operation.cpp)

add_executable(iceb_print ${SOURSE_BIN}) #сборка программы

target_link_libraries(iceb_print buh_g util_g ${GTK3_LIBRARIES} ) # Линковка программы с библиотеками

#****************************************************************
