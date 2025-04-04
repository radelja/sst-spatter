# Build and Installation Instructions for SST-Spatter

## External Dependent Components

### SST 14.1.0
See [Detailed Installation Instructions](https://sst-simulator.org/SSTPages/SSTBuildAndInstall_14dot1dot0_SeriesDetailedBuildInstructions/) for installing SST 14.1.0.

### Spatter 2.0.0+ (spatter-devel branch)
1. Clone the Spatter benchmark.
```
cd $HOME/scratch/src
git clone https://github.com/hpcgarage/spatter.git
cd spatter
git checkout spatter-devel
```

2. Set the home directory environment variable of the Spatter installation.
```
export SPATTER_HOME=$HOME/local/packages/spatter
```

3. Configure the build and installation of Spatter.
```
cmake -DCMAKE_INSTALL_PREFIX=$SPATTER_HOME -B build_serial -S .
```

4. Build and install Spatter.
```
cd build_serial
make all
make install
```

## SST-Spatter Build and Installation
1. Clone the SST-Spatter element.
```
cd $HOME/scratch/src
git clone https://github.com/hpcgarage/sst-spatter.git
cd sst-spatter
```
2. Set the home directory environment variable of the SST-Spatter installation.
```
export SST_SPATTER_HOME=$HOME/local/packages/sstspatter
```

3. Create a configure file.
```
./autogen.sh
```

4. Configure the build and installation of SST-Spatter.
```
./configure --prefix=$SST_SPATTER_HOME --with-sst-core=$SST_CORE_HOME \
  --with-spatter=$SPATTER_HOME
```

5. Build and install SST-Spatter.
```
make all
make install
```

6. Test your SST-Spatter install.
```
sst --version
sst-info
sst-test-elements -w "*spatter*"
```
