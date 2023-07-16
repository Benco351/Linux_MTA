#!/bin/bash
set -e 

echo -e "Hello Elior and Smadar here is the menu of commands\n"
echo -e "                                                                   
         1.sudo docker images - list all images\n
         2.sudo docker ps - list all alive proccess (look at status column)\n
         3.sudo docker container ls -a (-a= all) - list all containers *(not neccesary running)\n
         4.sudo docker stop {container ID} - stop a runing container\n
         5.sudo docker start {container ID }\n
         6.sudo docker remove {container ID} - removing the container from the docker\n
         7. IMPORTENT- sudo docker rmi -f  {Container ID/ name} (-f=force) - remove a Docker image\n "
echo "sleep for Ten seconds and then resume the Script";
sleep 3;         

VolumeName=$(date +%s);
VolumeName+="BenEliorSmadar";
echo $VolumeName;
if [ -x "$(command -v docker)" ]; then
    echo -e "Please check if docker updated";
    echo -e "continuing...\n";
else
    echo "Please install docker before using this script.";
fi
# VolumeChecker=$(sudo docker volume ls | grep $VolumeName 2>/dev/null);
docker volume create "$VolumeName";
if [ $? -eq 0 ]; then
    echo "Volume $VolumeName created succesfully";
fi
sleep 3

# if ["$VolumeChecker" = "" ]; then
#     echo "The volume does'nt exist, creating right now...";
# else
#     echo "Volume $VolumeName already exist, continuing...";
# fi
# need to add the docker pull command after uplodaing to docker hub
# sudo docker pull --------- our images.
# sudo docker pull ---------- our images.


#!!!!!!!!!!!!!!!!! need to delete when we upload to MAMA !!!!!!!!!!!!!!!!
##-----------------------------------------------------------

cd DBService/;
 docker build -t db_service .;
cd ..;
cd FlightsService/;
 docker build -t flights_service .;

if [ $? -eq 0 ]; then
    echo "Succesfully created both images, now running both...";
else
    echo "problem when creating an image - Call Ben Cohen or check internet :)";
fi
sleep 4

##-----------------------------------------------------------

docker run -d -v "$VolumeName":/shared-volume --name db_service --ipc=shareable  db_service;
if [ $? -eq 0 ]; then
    echo "Succesfully run db_service, want to interact ??";
    echo -e "\nUse command: sudo docker exec -it {container ID} /bin /bash\n";
    echo  "Keep in mind that because in Dockerfile the command CMD ["./DBService"]";
    echo -e "\nwritten so you cannot really interact and you must create brand new image without the CMD command on the dockerfile";
else
    echo "problem when creating an image - Call Ben Cohen or check internet :)";
fi
sleep 5;
docker run -it -v "$VolumeName":/shared-volume --name flights_service --ipc=container:db_service flights_service;
if [ $? -eq 0 ]; then
    echo "Succesfully run flights_service, want to interact ??";
    echo -e "\nUse command: sudo docker exec -it {container ID} /bin /bash\n";
    echo  "Keep in mind that because in Dockerfile the command CMD ["./Flightservice"]";
    echo -e "\nwritten so you cannot really interact and you must create brand new image without the CMD command on the dockerfile";
else
    echo "problem when creating an image - Call Ben Cohen or check internet :)";
fi

echo -e "\n\n There you go : \n";


