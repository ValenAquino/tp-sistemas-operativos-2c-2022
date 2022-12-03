#!/usr/bin/env bash

PRUEBAS_TLB_PATH=$(pwd)
cd ..

PRUEBAS_PATH=$(pwd)
cd ../GECK

PROJECT_PATH=$(pwd)

echo Yendo a la carpeta del proyecto. NOTA: Tiene que estar dentro de la carpeta de Documents.
echo $PROJECT_PATH

echo "Ejecutando GECK - Prueba TLB"
cd $PRUEBAS_PATH && sh ./run_all_by_config.sh tlb

sleep 5

cd $PROJECT_PATH

echo "Ejecutando Consola 1 en una terminal nueva..."
gnome-terminal --working-directory=$PROJECT_PATH/consola -e "./Debug/consola $PRUEBAS_TLB_PATH/consola_a.config $PRUEBAS_TLB_PATH/TLB_1"

echo "Ejecutando Consola 2 en una terminal nueva..."
gnome-terminal --working-directory=$PROJECT_PATH/consola -e "./Debug/consola $PRUEBAS_TLB_PATH/consola_b.config $PRUEBAS_TLB_PATH/TLB_2"

