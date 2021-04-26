TIMEOUT=$1
DATA="data/test5/*.wcnf"
for f in $DATA;
do
	echo "Processing $f...";
	timeout $1 ./main $f
done
