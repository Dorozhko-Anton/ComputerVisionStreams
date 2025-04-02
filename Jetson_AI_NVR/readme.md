# AI infused Network Video Recorder (AI-NVR) 

1. VST 
    - camera discovery
    - stream ingestion
    - storage
    - streaming
2. Deepstream perception service
    - PeopleNet2.6
    - DLA inference
3. Analytics (line crossing, region of interest)
    - latency vs accuracy tradeoff
4. Other services
    - ingress
    - redis
    - firewall
    - storage


Down ai_nvr package

```bash
wget --content-disposition 'https://api.ngc.nvidia.com/v2/resources/org/nvidia/team/jps/reference-workflow-and-resources/2.0.0/files?redirect=true&path=ai_nvr-2.0.0.tar.gz' -O ai_nvr-2.0.0.tar.gz

tar -xvf ai_nvr-2.0.0.tar.gz -C ./ai_nvr-2.0.0 --strip-components=1

sudo cp ai_nvr/config/ai-nvr-nginx.conf /opt/nvidia/jetson/services/ingress/config/

```


# Storage service 

https://docs.nvidia.com/jetson/jps/platform-services/storage.html

```bash
sudo cp ai_nvr/config/storage-quota.json /opt/nvidia/jetson-configs
```

# Networking service
https://docs.nvidia.com/jetson/jps/platform-services/networking.html
```bash
/opt/nvidia/jetson-configs/jetson-camif.conf
```

# Monitoring  (optional)

TODO: uncomment necessary lines
```
/opt/nvidia/jetson/services/ingress/config/platform-nginx.conf
```

# NVStream (optional)

# Mobile App

download from play store (https://play.google.com/store/apps/details?id=com.nvidia.ainvr)
direct access
cloud (not yet supported)

IP 192.168.1.66

# Run AI-NVR Application

 Install foundation services 
```bash
sudo apt update
sudo apt install nvidia-jetson-services
```

```bash
# sudo systemctl enable jetson-storage --now  # can brake filesystem, is you configured your harddrive manually you don't need it
# sudo systemctl enable jetson-networking --now
sudo systemctl enable jetson-redis --now
sudo systemctl enable jetson-monitoring --now
sudo systemctl enable jetson-sys-monitoring --now
sudo systemctl enable jetson-gpu-monitoring --now
```

reboot 

```
sudo reboot now
```

other services
```bash
sudo systemctl enable jetson-ingress --now
sudo systemctl enable jetson-vst --now
```

## check all enabled services 

```
sudo systemctl list-unit-files | grep jetson
```

## Scan opened ports 

```
nmap -p0- -v -A -T4 192.168.1.66
```

## Start application

```bash
cd ai_nvr

sudo docker compose -f compose_agx.yaml up -d --force-recreate
```

stop 
```bash
sudo docker compose -f compose_agx.yaml down --remove-orphans
```


Ports
1. 30000     VST
2. 3000   Grafana
3. 5000   Emdx (?)
4. 8081   Monitoring
5. 9091   Pushgateway
6. 30080  Ingress
 

# Actions:

1. Add streams to VST  (check out video about NVStreamer)
2. View video overlay & analytics https://docs.nvidia.com/jetson/jps/vst/VST_Quickstart_guide.html#web-ui-sample-label 
3. Use mobile app https://docs.nvidia.com/jetson/jps/mobile-app/app-overview.html

# How many streams a supported

1. Hardware platform
2. Model used (pruning, quantization, architecture)
3. DeepStream configuration (inference, tracker, streammux)
4. Stream resolution
5. Number of webRTC streams (RAM)

GPU, DLA, RAM, memory bandwidth, decode/encoder utilization

Example:  PeopleNet2.6 (DLA) + NVDCF tracker (PVA) = 16 H265 streams on AGX Orin + 2 WebRTC stream




# Analytics ( http://192.168.1.66:6001/api/v2/docs )

TODO: fix emdx behind ingress proxy https://fastapi.tiangolo.com/advanced/behind-a-proxy/#check-the-docs-ui 
# mdx webapi  ( http://192.168.1.66:5000/api/v2/docs )





# Metadata overlay 


Synchronize metadata and stream images


```python
# URL da imagem
url = "http://localhost:30080/vst/api/v1/live/stream/" + camera_id + "/picture?startTime=" + data['@timestamp']

# Faz a requisição para a URL
response = requests.get(url)
```

Plan:
1. add ts at batch meta level after nvstreammux
2. add this time stamp to minimal schema of DS
3. use this ts in redis message to retrieve image



```bash
wget --content-disposition 'https://api.ngc.nvidia.com/v2/resources/org/nvidia/team/jps/reference-workflow-and-resources/1.1.0/files?redirect=true&path=ai_nvr-1.1.0.tar.gz' -O ai_nvr-1.1.0.tar.gz

tar -xvf  ai_nvr-1.1.0.tar.gz -C ./ai_nvr-1.1.0 --strip-components=1


sudo cp ai_nvr-1.1.0/config/ai-nvr-nginx.conf /opt/nvidia/jetson/services/ingress/config/


cd ai_nvr-1.1.0
docker compose -f compose_agx_yolov8s.yaml up -d --force-recreate

```



# Logging and Observability 

Schema https://docs.nvidia.com/moj/deepstream/deepstream.html 

VST  192.168.1.66:30080/vst
REDIS 192.168.1.66:5540
SDR        (logs)
DEEPSTREAM (logs)


> read the logs from Metropolis (Redis, Ingress, VST) / DeepStream processes to troubleshoot the most common issues.



## Portainer
https://docs.portainer.io/start/install-ce/server/docker/linux 

```bash
docker volume create portainer_data
docker run -d -p 8000:8000 -p 9443:9443 --name portainer --restart=always -v /var/run/docker.sock:/var/run/docker.sock -v portainer_data:/data portainer/portainer-ce:lts
```

https://192.168.1.66:9443/


1. Stacks

AI_NVR
REDIS
VST
monitoring
ingress

2. AI_NVR Stack

deepstream
emdx-webapi
sdr-emdx


## Redis Insight
https://hub.docker.com/r/redis/redisinsight 

```bash
docker run -d --name redisinsight -p 5540:5540 redis/redisinsight:latest -v redisinsight:/data
```

http://192.168.1.66:5540/ 