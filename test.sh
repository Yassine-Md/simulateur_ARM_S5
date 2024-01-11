#!/bin/bash

if [ $# -ne 1 ]
then
	echo "[Fichier a tester] ERREUR. 
        nom de fichier .s a executer !"
else
	#filename=$(basename $1)
	filename=$1

	# Recupere le numero de la derniere ligne : swi 0x123456
	lastLine=$(grep -n 'swi 0x123456' $filename'.s' | awk -F ":" '{print $1}')	
	if [ -f "arm_simulator" ] #verifier si l'executable est present
	then
		> log.out
		echo "before"
		./arm_simulator --gdb-port 44000 &
		gdb-multiarch -ex "set pagination off" \
		-ex "file $filename" \
		-ex "target remote localhost:44000" \
        -ex "set endian big" \
		-ex "load" \
		-ex "b $lastLine" \
		-ex "continue" \
		-ex "set logging file log_gdb.txt" \
    	-ex "set logging enabled on" \
		-ex "info registers" \
		-ex "set logging enabled off" \

		echo "---------------------{RESULTAT}----------------------"
		cat log_gdb.txt
		echo "------------------------------------------------"

	else
		echo "[Erreur] l'executable du simulateur n'est pas present dans le dossier courant."
	fi
fi