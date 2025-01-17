# Docs

https://developer.nvidia.com/triton-inference-server



# Docker Setup

quickstart https://docs.nvidia.com/deeplearning/triton-inference-server/user-guide/docs/getting_started/quickstart.html 

1. Choose container https://catalog.ngc.nvidia.com/orgs/nvidia/containers/tritonserver 

in 90% use cases we need TensoRT, ONNX, PyTorch support so

```bash
docker pull  nvcr.io/nvidia/tritonserver:24.06-py3
```

For Jetson devices 

```
docker pull nvcr.io/nvidia/tritonserver:24.06-py3-igpu
```

# Create a model repository 

example: https://github.com/triton-inference-server/server/tree/main/docs/examples/model_repository 

```bash
mkdir -p model_repository/densenet_onnx/1
wget -O model_repository/densenet_onnx/1/model.onnx \
     https://github.com/onnx/models/raw/main/validated/vision/classification/densenet-121/model/densenet-7.onnx
```

# Run standalone Triton
```bash
docker run --gpus=1 --rm -p8000:8000 -p8001:8001 -p8002:8002 --name triton -v $PWD/model_repository:/models nvcr.io/nvidia/tritonserver:24.06-py3 tritonserver --model-repository=/models
```

# Run docker compose with metrics

```
docker compose up -d
```


# Check the triton server is running and ready to serve

```bash
#docker logs -f triton
#docker compose ps
# All the models should show “READY” status
 curl -v localhost:8000/v2/health/ready
```


# Run test inference with client image

```bash
docker run -it --rm --net=host nvcr.io/nvidia/tritonserver:24.06-py3-sdk
/workspace/install/bin/image_client -m densenet_onnx -c 3 -s INCEPTION /workspace/images/mug.jpg
```



# Grafana dashboard

https://grafana.com/docs/grafana/latest/setup-grafana/installation/docker/

Community dashboards

https://grafana.com/grafana/dashboards/16231-triton/
https://grafana.com/grafana/dashboards/12832-triton-inference-server/ 
https://grafana.com/grafana/dashboards/18027-kserve-triton-latency/


Metrics guide to create your own dashboard
https://docs.nvidia.com/deeplearning/triton-inference-server/user-guide/docs/user_guide/metrics.html 





# Other Triton related products

## Performance Analyzer 

https://docs.nvidia.com/deeplearning/triton-inference-server/user-guide/docs/perf_benchmark/perf_analyzer.html


## Model Navigator

https://docs.nvidia.com/deeplearning/triton-inference-server/user-guide/docs/model_navigator/README.html

## Yolo v11 on triton 

https://docs.ultralytics.com/guides/triton-inference-server/#running-triton-inference-server
