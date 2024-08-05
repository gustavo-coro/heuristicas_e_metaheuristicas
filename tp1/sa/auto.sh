NUM_RUNS=10

FILE_ENTRADA="tsp_51"
DATA_FILE="testes/pcv/teste13.txt"
ITERACOES=1000
TEMPERATURA=800
TAXA_RESFRIAMENTO=0.999

# Create and write to the data file
touch $DATA_FILE
printf "$ITERACOES\n$TEMPERATURA\n$TAXA_RESFRIAMENTO\n" >> $DATA_FILE

for (( i=1; i<=$NUM_RUNS; i++ ))
do
    echo "Generating data for run $i..."
    ./pcv ../entradas/pcv/$FILE_ENTRADA $ITERACOES $TEMPERATURA $TAXA_RESFRIAMENTO $DATA_FILE
done