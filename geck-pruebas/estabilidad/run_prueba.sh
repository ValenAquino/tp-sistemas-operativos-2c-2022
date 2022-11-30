#!/usr/bin/env bash

PRUEBAS_ESTABILIDAD_PATH=$(pwd)
cd ..

PRUEBAS_PATH=$(pwd)
cd ../GECK

PROJECT_PATH=$(pwd)

echo Yendo a la carpeta del proyecto. 
echo $PROJECT_PATH

echo "Ejecutando GECK - Prueba ESTABILIDAD"
cd $PRUEBAS_PATH && sh ./run_all_by_config.sh estabilidad

sleep 5

cd $PROJECT_PATH

echo "Ejecutando Consola 1 en una terminal nueva..."
gnome-terminal --working-directory=$PROJECT_PATH/consola -e "valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./Debug/consola $PRUEBAS_ESTABILIDAD_PATH/consola_a.config $PRUEBAS_ESTABILIDAD_PATH/ESTABILIDAD_1"

echo "Ejecutando Consola 2 en una terminal nueva..."
gnome-terminal --working-directory=$PROJECT_PATH/consola -e "valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./Debug/consola $PRUEBAS_ESTABILIDAD_PATH/consola_b.config $PRUEBAS_ESTABILIDAD_PATH/ESTABILIDAD_2"

echo "Ejecutando Consola 3 en una terminal nueva..."
gnome-terminal --working-directory=$PROJECT_PATH/consola -e "valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./Debug/consola $PRUEBAS_ESTABILIDAD_PATH/consola_c.config $PRUEBAS_ESTABILIDAD_PATH/ESTABILIDAD_3"

echo "Ejecutando Consola 4 en una terminal nueva..."
gnome-terminal --working-directory=$PROJECT_PATH/consola -e "valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./Debug/consola $PRUEBAS_ESTABILIDAD_PATH/consola_d.config $PRUEBAS_ESTABILIDAD_PATH/ESTABILIDAD_4"

echo "Ejecutando Consola 5 en una terminal nueva..."
gnome-terminal --working-directory=$PROJECT_PATH/consola -e "valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./Debug/consola $PRUEBAS_ESTABILIDAD_PATH/consola_e.config $PRUEBAS_ESTABILIDAD_PATH/ESTABILIDAD_5"

echo "Ejecutando Consola 6 en una terminal nueva..."
gnome-terminal --working-directory=$PROJECT_PATH/consola -e "valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./Debug/consola $PRUEBAS_ESTABILIDAD_PATH/consola_f.config $PRUEBAS_ESTABILIDAD_PATH/ESTABILIDAD_6"