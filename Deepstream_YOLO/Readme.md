# Start docker 

```bash
docker build -t deepstream:7.0 .
# Step to run the docker
export DISPLAY=:0
xhost +
docker run -it --rm --net=host --gpus all -e DISPLAY=$DISPLAY --device /dev/snd -v /tmp/.X11-unix/:/tmp/.X11-unix -v $PWD:/workspace deepstream:7.0 bash
```


# Export yolo model and test with sample add

```bash
conda create -n yoloexport python=3.10

conda init
bash
conda activate yoloexport

cd /workspace/build/
git clone https://github.com/ultralytics/ultralytics
cd /ultralytics
pip install -e ".[export]" onnxslim

cd /workspace/build/
git clone https://github.com/marcoslucianops/DeepStream-Yolo
cp /workspace/build/DeepStream-Yolo/utils/export_yolo11.py /workspace/build/ultralytics

# download model weights
# from https://github.com/ultralytics/assets/releases
cd /workspace/build/ultralytics/
wget https://github.com/ultralytics/assets/releases/download/v8.3.0/yolo11s.pt

python3 export_yolo11.py -w yolo11s.pt --opset 17 -s 1280 --simplify --dynamic
cp yolo11s.pt.onnx labels.txt /workspace/build/DeepStream-Yolo

export CUDA_VER=12.1
export LIBRARY_PATH=/usr/local/cuda/lib64:${LIBRARY_PATH}
cd /workspace/build/DeepStream-Yolo
make -C nvdsinfer_custom_impl_Yolo clean && make -C nvdsinfer_custom_impl_Yolo


nano config_infer_primary_yolo11.txt 
nano deepstream_app_config.txt

deepstream-app -c deepstream_app_config.txt
# GST_DEBUG=*:3 deepstream-app -c deepstream_app_config.txt


# perf analysis of engine
/usr/src/tensorrt/bin/trtexec --loadEngine=model_b1_gpu0_fp32.engine
```

# Build custom pipeline with YOLO 

```bash
cd /workspace/
mkdir build && cd build && cmake .. && make

./OldDeepstream
```


Links:
1. https://wiki.seeedstudio.com/YOLOv8-DeepStream-TRT-Jetson/
2. https://github.com/levipereira/deepstream-yolo-e2e
3. https://github.com/marcoslucianops/DeepStream-Yolo
4. https://docs.ultralytics.com/guides/deepstream-nvidia-jetson