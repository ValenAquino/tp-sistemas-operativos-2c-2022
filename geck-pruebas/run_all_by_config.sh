#!/usr/bin/env bash

PRUEBAS_PATH=$(pwd)
cd ../GECK
PROJECT_PATH=$(pwd)

# el parametro 1 deberia ser el nombre de la carpeta de las pruebas que se van a correr.

echo Yendo a la carpeta del proyecto. NOTA: Tiene que estar dentro de la carpeta de Documents.
echo $PROJECT_PATH
cd $PROJECT_PATH

echo Instalando librerias...
make install

echo Limpiando proyectos...
make clean

echo Compilando proyectos...
make all

echo Ejecutando Memoria en una terminal nueva...
gnome-terminal --working-directory=$PROJECT_PATH/memoria -e "./Debug/memoria $PRUEBAS_PATH/$1/memoria.config"

echo Ejecutando CPU en una terminal nueva...
gnome-terminal --working-directory=$PROJECT_PATH/cpu -e "./Debug/cpu $PRUEBAS_PATH/$1/cpu.config"

echo Ejecutando Kernel en una terminal nueva...
gnome-terminal --working-directory=$PROJECT_PATH/kernel -e "valgrind ./Debug/kernel $PRUEBAS_PATH/$1/kernel.config"

