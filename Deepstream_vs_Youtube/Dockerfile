FROM nvcr.io/nvidia/deepstream:6.2-samples

RUN /opt/nvidia/deepstream/deepstream/user_additional_install.sh
RUN apt-get update -y && apt-get install -y libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer-plugins-bad1.0-dev gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa gstreamer1.0-gl gstreamer1.0-gtk3 gstreamer1.0-qt5 gstreamer1.0-pulseaudio

# install youtube dl
RUN apt-get install -y python-pip
RUN pip install git+https://github.com/ytdl-org/youtube-dl.git@master#egg=youtube_dl