

#   Send data from an ascii file to Graphana 
#   R. Lica - 28.11.2017
 

#   Install prerequisites 
#   'sudo apt install inotify-tools bc curl'


FILECHANGE=""/Data1/GeckoPlots/Germanium\*.txt""
GERMANIUM=""/Data1/GeckoPlots/Germanium\ Plots.txt""
LABR=""/Data1/GeckoPlots/LaBr\ Plots.txt""
BGO=""/Data1/GeckoPlots/BGO\ Plots.txt""
NEUTRON=""/Data1/GeckoPlots/Neutron\ Plots.txt""
SI=""/Data1/GeckoPlots/Si\ Plots.txt""

# PathToDB='http://influx.kern.phys.au.dk/write?db=ids'
#PathToDB='http://localhost:8086/write?db=dfn'
# PathToDB='http://192.168.164.35:8086/write?db=rosphere_influxdb'

 PathToDB='http://prisma.nipne.ro:8086/write?db=dfn'
# PathToDB='http://194.102.59.12:8086/write?db=rosphere_influxdb'



# ServerAuth='myusername:mypassword' // authentication for db
ServerAuth='dfnuser:influx10'

RAW_PATH="$(ls -lt /Data*/ | head -1 | tr -d ":")" 

# Write a point to the database mydb using basic authentication
#  curl -i -XPOST -u myusername:mypassword "http://localhost:8086/write?db=mydb" --data-binary 'mymeas,mytag=1 myfield=91'
# Write a point to the database mydb using hhtp authetication
# curl -i -XPOST "http://localhost:8086/write?db=mydb&u=myusername&p=mypassword" --data-binary 'mymeas,mytag=1 myfield=91'


while inotifywait -q -e modify $FILECHANGE >/dev/null; 
	do echo "Sending data to graphana"
	
	#EXTRACTING the YIELD 
	TRIGRATE=$(awk 'NR==1{print $1}' "$GERMANIUM")
	TRIGGER=$(awk 'NR==1{print $2}' "$GERMANIUM")
	TOTALHPGE=$(awk 'NR==2{print $1}' "$GERMANIUM")
	HPGE1=$(awk 'NR==3{print $1}' "$GERMANIUM")
	HPGE2=$(awk 'NR==3{print $2}' "$GERMANIUM")
	HPGE3=$(awk 'NR==3{print $3}' "$GERMANIUM")
	HPGE4=$(awk 'NR==3{print $4}' "$GERMANIUM")
	HPGE5=$(awk 'NR==3{print $5}' "$GERMANIUM")
	HPGE6=$(awk 'NR==3{print $6}' "$GERMANIUM")
	HPGE7=$(awk 'NR==3{print $7}' "$GERMANIUM")
	HPGE8=$(awk 'NR==3{print $8}' "$GERMANIUM")
	HPGE9=$(awk 'NR==3{print $9}' "$GERMANIUM")
	HPGE10=$(awk 'NR==3{print $10}' "$GERMANIUM")
	HPGE11=$(awk 'NR==3{print $11}' "$GERMANIUM")
	HPGE12=$(awk 'NR==3{print $12}' "$GERMANIUM")
	HPGE13=$(awk 'NR==3{print $13}' "$GERMANIUM")
	HPGE14=$(awk 'NR==3{print $14}' "$GERMANIUM")
	HPGE15=$(awk 'NR==3{print $15}' "$GERMANIUM")
	TOTALBGO=$(awk 'NR==2{print $1}' "$BGO")
	BGO1=$(awk 'NR==3{print $1}' "$BGO")
	BGO2=$(awk 'NR==3{print $2}' "$BGO")
	BGO3=$(awk 'NR==3{print $3}' "$BGO")
	BGO4=$(awk 'NR==3{print $4}' "$BGO")
	BGO5=$(awk 'NR==3{print $5}' "$BGO")
	BGO6=$(awk 'NR==3{print $6}' "$BGO")
	BGO7=$(awk 'NR==3{print $7}' "$BGO")
	BGO8=$(awk 'NR==3{print $8}' "$BGO")
	BGO9=$(awk 'NR==3{print $9}' "$BGO")
	BGO10=$(awk 'NR==3{print $10}' "$BGO")
	BGO11=$(awk 'NR==3{print $11}' "$BGO")
	BGO12=$(awk 'NR==3{print $12}' "$BGO")
	BGO13=$(awk 'NR==3{print $13}' "$BGO")
	BGO14=$(awk 'NR==3{print $14}' "$BGO")
	BGO15=$(awk 'NR==3{print $15}' "$BGO")
	TOTALSI=$(awk 'NR==2{print $1}' "$SI")
	SI1=$(awk 'NR==3{print $1}' "$SI")
	SI2=$(awk 'NR==3{print $2}' "$SI")
	SI3=$(awk 'NR==3{print $3}' "$SI")
	SI4=$(awk 'NR==3{print $4}' "$SI")
	SI5=$(awk 'NR==3{print $5}' "$SI")
	SI6=$(awk 'NR==3{print $6}' "$SI")
	TOTALLA=$(awk 'NR==2{print $1}' "$LABR")
	La01=$(awk 'NR==3{print $1}' "$LABR")
	La02=$(awk 'NR==3{print $2}' "$LABR")
	La03=$(awk 'NR==3{print $3}' "$LABR")
	La04=$(awk 'NR==3{print $4}' "$LABR")
	La05=$(awk 'NR==3{print $5}' "$LABR")
	La06=$(awk 'NR==3{print $6}' "$LABR")
	La07=$(awk 'NR==3{print $7}' "$LABR")
	La08=$(awk 'NR==3{print $8}' "$LABR")
	La09=$(awk 'NR==3{print $9}' "$LABR")
	La10=$(awk 'NR==3{print $10}' "$LABR")
	
	EXP_NAME="$(basename $(ls -I *.txt -I Online -t $RAW_PATH | head -1))"
        FILE_NAME="$(basename $(ls -I *.txt -I Online -t $RAW_PATH$EXP_NAME/ | head -1))"

		
	echo "============================================"
	tput setaf 2; echo " RAW PATH =   $RAW_PATH"
	tput setaf 2; echo " EXP PATH =   $EXP_NAME"
	tput setaf 2; echo " FILENAME =   $FILE_NAME"
	tput setaf 2; echo " TRIGGER RATE (cps) =   $TRIGRATE"
	tput sgr0
	echo "============================================"

    	#bash handles only integer maths but you can use bc

			
		curl --user $ServerAuth -i -XPOST $PathToDB --data-binary "dfn file_name=\"$FILE_NAME\"
dfn trigRate=$TRIGRATE 
dfn trigger=$TRIGGER 
dfn totalHPGe=$TOTALHPGE
dfn totalBGO=$TOTALBGO
dfn totalSI=$TOTALSI
dfn totalLa=$TOTALLA
dfn HPGe1=$HPGE1
dfn HPGe2=$HPGE2
dfn HPGe3=$HPGE3
dfn HPGe4=$HPGE4
dfn HPGe5=$HPGE5
dfn HPGe6=$HPGE6
dfn HPGe7=$HPGE7
dfn HPGe8=$HPGE8 
dfn HPGe9=$HPGE9 
dfn HPGe10=$HPGE10 
dfn HPGe11=$HPGE11 
dfn HPGe12=$HPGE12 
dfn HPGe13=$HPGE13 
dfn HPGe14=$HPGE14 
dfn HPGe15=$HPGE15 
dfn BGO1=$BGO1
dfn BGO2=$BGO2
dfn BGO3=$BGO3
dfn BGO4=$BGO4
dfn BGO5=$BGO5
dfn BGO6=$BGO6
dfn BGO7=$BGO7
dfn BGO8=$BGO8
dfn BGO9=$BGO9
dfn BGO10=$BGO10 
dfn BGO11=$BGO11 
dfn BGO12=$BGO12 
dfn BGO13=$BGO13 
dfn BGO14=$BGO14 
dfn BGO15=$BGO15
dfn SI1=$SI1
dfn SI2=$SI2
dfn SI3=$SI3
dfn SI4=$SI4
dfn SI5=$SI5
dfn SI6=$SI6
dfn La01=$La01
dfn La02=$La02
dfn La03=$La03
dfn La04=$La04
dfn La05=$La05
dfn La06=$La06
dfn La07=$La07
dfn La08=$La08
dfn La09=$La09
dfn La10=$La10

" # >> /dev/null 
	
	
	sleep 3

done

