#!/bin/bash

echo "Generating reference files..."
# Miranda
echo "Spatter..."
sst spatterbench.py -- -pUNIFORM:8:1 -kgather -l$((2**16)) \
    --statfile refFiles/test_miranda_spatterbench_gather_stats.csv \
    > refFiles/test_miranda_spatterbench_gather.out &
sst spatterbench.py -- -pUNIFORM:8:1 -kscatter -l$((2**16)) \
    --statfile refFiles/test_miranda_spatterbench_scatter_stats.csv \
    > refFiles/test_miranda_spatterbench_scatter.out &
sst spatterbench.py -- -gUNIFORM:8:1 -uUNIFORM:8:1 -kgs -l$((2**16)) \
    --statfile refFiles/test_miranda_spatterbench_gs_stats.csv \
    > refFiles/test_miranda_spatterbench_gs.out &
sst spatterbench.py -- -pUNIFORM:8:1 -uUNIFORM:8:1 -kmultiscatter -l$((2**16)) \
    --statfile refFiles/test_miranda_spatterbench_multiscatter_stats.csv \
    > refFiles/test_miranda_spatterbench_multiscatter.out &
sst spatterbench.py -- -pUNIFORM:8:1 -gUNIFORM:8:1 -kmultigather -l$((2**16)) \
    --statfile refFiles/test_miranda_spatterbench_multigather_stats.csv \
    > refFiles/test_miranda_spatterbench_multigather.out &
wait
