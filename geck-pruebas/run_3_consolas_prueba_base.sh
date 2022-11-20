#!/usr/bin/env bash


PROJECT_PATH=~/Documents/tp-2022-2c-Compilant/GECK
PRUEBAS_PATH=~/Documents/tp-2022-2c-Compilant/geck-pruebas

echo Yendo a la carpeta del proyecto. NOTA: Tiene que estar dentro de la carpeta de Documents.
echo $PROJECT_PATH
cd $PROJECT_PATH

echo "Ejecutando Consola 1 en una terminal nueva..."
gnome-terminal --working-directory=$PROJECT_PATH/consola -e "./Debug/consola $PROJECT_PATH/consola/consola.config  $PRUEBAS_PATH/BASE_1"

echo "Ejecutando Consola 2 en una terminal nueva..."
gnome-terminal --working-directory=$PROJECT_PATH/consola -e "./Debug/consola $PROJECT_PATH/consola/consola.config $PRUEBAS_PATH/BASE_2"

echo "Ejecutando Consola 3 en una terminal nueva..."
gnome-terminal --working-directory=$PROJECT_PATH/consola -e "./Debug/consola $PROJECT_PATH/consola/consola.config $PRUEBAS_PATH/BASE_3"

