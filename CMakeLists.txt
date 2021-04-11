#cборка iceBw

project(iceBw)

cmake_minimum_required(VERSION 2.6)

message ("Начали построение")

#Используйте пакет pkg-config для обнаружения GTK + заголовки / файлы библиотеки
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

#Установка CMake использовать GTK +, указать компилятору, где искать
#заголовки и компоновщику где искать библиотеки
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})

include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})

#Добавьте другие флаги компилятора
add_definitions(${GTK3_CFLAGS_OTHER})


#set(MYSQLCLIENT_LIB mysqlclient) #расположение библиотеки mysqlclient
set(MYSQLCLIENT_LIB mariadbclient) #расположение библиотеки mariadb
#set(MYSQLCLIENT_LIB mariadb) #расположение библиотеки mariadb


#перечень каталогов где искать инклуде файлы
include_directories(./ buhg_g libbuh_g libutil_g libsql util /usr/include/mysql /usr/include/mariadb) # Расположение заголовочных файлов

#add_definitions(-Wall -O2 `pkg-config --cflags gtk+-3.0`) #добавить директивы компилятора. Здесь мы включаем оптимизацию O2 и разрешаем все предупреждающие сообщения.
add_definitions(-Wall -O2) #добавить директивы компилятора. Здесь мы включаем оптимизацию O2 и разрешаем все предупреждающие сообщения.


add_subdirectory (./libsql)
add_subdirectory (./libutil_g)
add_subdirectory (./libbuh_g)
add_subdirectory (./buhg_g)

add_subdirectory (./util)
add_subdirectory (./additionally)

