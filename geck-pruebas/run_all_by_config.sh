#!/usr/bin/env bash


PROJECT_PATH=~/Documents/tp-2022-2c-Compilant/GECK

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
gnome-terminal --working-directory=$PROJECT_PATH/memoria -e "./Debug/memoria $PROJECT_PATH/memoria/$1"

echo Ejecutando CPU en una terminal nueva...
gnome-terminal --working-directory=$PROJECT_PATH/cpu -e "./Debug/cpu $PROJECT_PATH/cpu/$1"

echo Ejecutando Kernel en una terminal nueva...
gnome-terminal --working-directory=$PROJECT_PATH/kernel -e "./Debug/kernel $PROJECT_PATH/kernel/$1"
