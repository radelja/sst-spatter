#!/bin/bash

echo "Generating reference files..."
# Miranda
echo "Spatter..."
sst sst_spatter.py -- -pUNIFORM:8:1 -kgather -l$((2**16)) \
    --statfile refFiles/test_sst_spatter_gather_stats.csv \
    > refFiles/test_sst_spatter_gather.out &
sst sst_spatter.py -- -pUNIFORM:8:1 -kscatter -l$((2**16)) \
    --statfile refFiles/test_sst_spatter_scatter_stats.csv \
    > refFiles/test_sst_spatter_scatter.out &
sst sst_spatter.py -- -gUNIFORM:8:1 -uUNIFORM:8:1 -kgs -l$((2**16)) \
    --statfile refFiles/test_sst_spatter_gs_stats.csv \
    > refFiles/test_sst_spatter_gs.out &
sst sst_spatter.py -- -pUNIFORM:8:1 -uUNIFORM:8:1 -kmultiscatter -l$((2**16)) \
    --statfile refFiles/test_sst_spatter_multiscatter_stats.csv \
    > refFiles/test_sst_spatter_multiscatter.out &
sst sst_spatter.py -- -pUNIFORM:8:1 -gUNIFORM:8:1 -kmultigather -l$((2**16)) \
    --statfile refFiles/test_sst_spatter_multigather_stats.csv \
    > refFiles/test_sst_spatter_multigather.out &
wait
