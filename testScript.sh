TIMEOUT=$1
DATA="data/test_06_05/*.wcnf"

for f in $DATA;
do
  echo "----------------------------------"
  echo "Processing $f"
  for i in {1..10}
  do
    echo "[Iter $i/10]";
    #timeout "$TIMEOUT" ./main "$f" "$i"
    ./main "$f" "$i"
  done
done
