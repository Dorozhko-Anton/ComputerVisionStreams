FROM nvcr.io/nvidia/deepstream:7.0-samples-multiarch

RUN /opt/nvidia/deepstream/deepstream/user_additional_install.sh
RUN apt-get update -y && apt-get install -y libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer-plugins-bad1.0-dev gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa gstreamer1.0-gl gstreamer1.0-gtk3 gstreamer1.0-qt5 gstreamer1.0-pulseaudio


RUN wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh -O ~/miniconda.sh
RUN bash ~/miniconda.sh -b -p $HOME/miniconda

ENV PATH="/root/miniconda3/bin:${PATH}"
RUN wget \
    https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh \
    && mkdir -p /root/.conda \
    && bash Miniconda3-latest-Linux-x86_64.sh -b -p /root/miniconda3 \
    && rm -f Miniconda3-latest-Linux-x86_64.sh

RUN conda --version
RUN conda install jupyterlab notebook  -c conda-forge -y

RUN apt install -y git cmake
RUN pip install git+https://github.com/ytdl-org/youtube-dl.git@master#egg=youtube_dl
RUN pip install matplotlib


WORKDIR /
RUN git clone -b deepstream-fixes https://github.com/Dorozhko-Anton/TrackEval
WORKDIR /workspace

CMD ["/root/miniconda3/bin/jupyter","notebook", "--allow-root"]