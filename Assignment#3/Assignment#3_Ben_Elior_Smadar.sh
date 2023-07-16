#!/bin/bash
set -e   

if [ -x "$(command -v docker)" ]; then
    echo -e "Please check if docker updated";
    echo -e "continuing...\n";
else
    echo "Please install docker before using this script.";
fi


docker pull benco646/dbservice
if [ $? -eq 0 ]; then
    echo -e "Succesfully pulled dbService image\n";
else
    echo -e "problem when pulling the dbService image\n";
fi
docker pull benco646/flightsservice
if [ $? -eq 0 ]; then
    echo -e "\nSuccesfully pulled flightsservice image\n";
else
    echo -e "\nproblem when pulling the flightsservice image\n";
fi



docker run -d -v /tmp:/tmp --name dbService --ipc=host benco646/dbservice;
if [ $? -eq 0 ]; then
    echo -e "\nSuccesfully run dbService\n";
else
    echo -e "\nproblem when running the dbService image\n";
fi

docker run -it -v /tmp:/tmp --name FlightsService --ipc=host benco646/flightsservice;


