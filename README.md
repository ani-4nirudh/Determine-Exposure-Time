# Description
- This repo contains source code to calculate Mean Intensity Gradient (MIG) using OpenCV.
- The results for the MIG are saved inside the 'MIG_results' folder.

# Installation
- Go to project directory and create folders `lib` and `build`:
  ```
  cd <your_path>/img_acq_testing
  mkdir -p build lib
  ```
- Install ZLIB: `sudo apt install zlib1g-dev`
- Install LibXLSXWriter from instructions [here](https://libxlsxwriter.github.io/getting_started.html). Make it on your own from given instructions inside your `lib` directory. 

# Build
- Copy the 'images' directory from `init_img_acq` to `init_mig_exp_time` directory.
- Makefile generation and running the executable:
  ```
  cd build
  cmake -s ..
  make -j
  ./init_mig_exp_time
  ```