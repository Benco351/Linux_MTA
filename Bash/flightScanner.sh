#!/bin/bash
set -e
Location=$(pwd);

handleTerminal()
{
    echo "retrieving $1 flights information...";
    if [ ! -d "flightsDB" ]; then
        mkdir flightsDB;
        cd flightsDB/;
    fi
    WorkingFolder=$Location/flightsDB/$1;
    CurrentUnixTime=$(date +%s); #getting now unix time
    YesterdateUnixTime=$(($CurrentUnixTime - 86400)); #getting 24h ago unix time
    cd $Location/flightsDB/;
    if [ ! -d "$WorkingFolder" ]; then 
        mkdir $1
        cd $1/
    fi
       
    if [ ! -f "$WorkingFolder/$1.arv" ]; then 
        touch $WorkingFolder/$1.arv;
    fi
    
    if [ ! -f "$WorkingFolder/$1.dpt" ]; then 
        touch $WorkingFolder/$1.dpt;
    fi
    handleArrivalApi $ICOA $YesterdateUnixTime $CurrentUnixTime $WorkingFolder;
    handleDepartureApi $ICOA $YesterdateUnixTime $CurrentUnixTime $WorkingFolder;
}     
handleArrivalApi()
{
  http_codeN_File="$(curl -w "%{http_code}" -o $1.arvjson -s "https://opensky-network.org/api/flights/arrival?airport=$1&begin=$2&end=$3")";
   if [ $http_codeN_File -eq "200" ]; then # Check if the curl command exited successfully
        OrderJson $1 "arv";
        rm $1.arvjson;
   else
       echo "$http_codeN_File status error when retrieving arrival flights information for airport $1";
   fi
}
handleDepartureApi()
{  
   http_codeN_File="$(curl -w "%{http_code}" -o $1.dptjson -s "https://opensky-network.org/api/flights/departure?airport=$1&begin=$2&end=$3")"; 
   if [ $http_codeN_File -eq "200" ]; then # Check if the curl command exited successfully
        OrderJson $1 "dpt";
        rm $1.dptjson;
   else
       echo "$http_codeN_File status error when retrieving departure flights information for airport $1";
   fi
}
OrderJson()
{
    if [ ! -f "$1.$2json" ]; then
        echo "Error: File $1.$2json not found";
        return 1;
    fi
    cat $1.$2'json' | sed 's/,/\n/g' | sed 's/{/{\n/g' | sed 's/]//g' | sed 's/"//g'| sed 's/,/\n/g' | sed 's/{/{\n/g' | sed 's/]//g' | sed 's/"//g' | sed 's/}//' | tail -n 13 | awk -F: '{print $1}' | sed -z 's/\n/,/g;s/,$/\n/' | awk -F',' '{print $2,$3,$4,$5,$6,$7}' | sed 's/ /,/g' > $1.$2;
    if [ ! -s "$1.$2" ]; then
        echo "Error: Output file $1.$2 is empty"
        return 1
    fi
    cat $1.$2'json' | sed 's/,/\n/g' | sed 's/{/{\n/g' | sed 's/]//g' | sed 's/"//g'| awk -F: '{print $2}' | sed -z 's/\n/,/g;s/,$/\n/' | sed 's/},/\n/g'| sed  's/ //g' | sed 's/}$//' |sed 's/^,//g'  |awk -F',' '{print $1,$2,$3,$4,$5}'| sed 's/ /,/g' >> $1.$2;
}

for ICOA in "$@"
do
    handleTerminal $ICOA //LLBG
done
