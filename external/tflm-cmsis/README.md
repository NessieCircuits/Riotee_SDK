# Tensorflow Light for Microcontrollers with CMSIS-NN Kernel

This directory contains the precompiled library and headers for Tensorflow Light for Microcontrollers with the CMSIS-NN Kernel.

To generate the tree from the upstream repository:

```bash
git clone https://github.com/tensorflow/tflite-micro.git
cd tflite-micro
make -j4 -f tensorflow/lite/micro/tools/make/Makefile OPTIMIZED_KERNEL_DIR=cmsis_nn TARGET=cortex_m_generic TARGET_ARCH=cortex-m4+fp TOOLCHAIN=gcc microlite
cp gen/cortex_m_generic_cortex-m4+fp_default/lib/libtensorflow-microlite.a [RIOTEE_SDK_PATH]/external/tflm-cmsis
python tensorflow/lite/micro/tools/project_generation/create_tflm_tree.py ./tree --makefile_options "TARGET=cortex_m_generic TARGET_ARCH=cortex-m4+fp OPTIMIZED_KERNEL_DIR=cmsis_nn TARGET_TOOLCHAIN_ROOT=/usr/bin/"
cd tree
find . -name '*.h' | cpio -pdm [RIOTEE_SDK_PATH]/external/tflm-cmsis
```
