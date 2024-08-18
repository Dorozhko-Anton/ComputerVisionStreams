# Docker installation

https://docs.nvidia.com/metropolis/deepstream/dev-guide/text/DS_docker_containers.html 

1. install `docker`
2. install `nvidia-container-toolkit` 
3. get `NGC` account
4. get `NGC_API_KEY`  
5. `docker login nvcr.io`

```bash
Username: "$oauthtoken"
Password: "YOUR_NGC_API_KEY"
```

# Run docker with deepstream

https://catalog.ngc.nvidia.com/orgs/nvidia/containers/deepstream 

```
docker login nvcr.io
docker build -t deepstream:7.0-trackeval .
```

```
# Step to run the docker
export DISPLAY=:0
xhost +
docker run -it --rm --net=host --gpus all -e DISPLAY=$DISPLAY --device /dev/snd -v /tmp/.X11-unix/:/tmp/.X11-unix -v $PWD:/workspace deepstream:7.0-trackeval bash
```


# Execute

```bash
apt-get install nlohmann-json3-dev
pip install scipy numpy pandas

cd /workspace/cpp_pipeline_old/
mkdir -p build && cd build && cmake .. && make


./OldDeepstream /workspace/data/config1.json 
./OldDeepstream /workspace/data/config2.json 
```

# Convert pipeline output to MOT

```bash
bash  /workspace/data/kittiTrack2mot.sh -i ./output/Retail_Synthetic_Cam01 -o ./output/Retail_Synthetic_Cam01.txt; 
bash  /workspace/data/kittiTrack2mot.sh -i ./output/Retail_Synthetic_Cam02 -o ./output/Retail_Synthetic_Cam02.txt; 
```

# Evaluate with TrackEval 

```bash
cd output

python3 /TrackEval/scripts/run_mot_challenge.py --USE_PARALLEL False --GT_FOLDER /workspace/data/SDG_1min_videos --TRACKERS_FOLDER "./" --TRACKERS_TO_EVAL "./" --BENCHMARK "" --TRACKER_SUB_FOLDER "" --METRICS Identity CLEAR HOTA --SPLIT_TO_EVAL "" --SEQMAP_FILE /workspace/data/SDG_1min_all_2.txt 2>&1 | tee trackeval.txt


python3 /workspace/parse_trackeval_results.py

cat HOTA.csv
cat CLEAR.csv
cat IDF.csv
```