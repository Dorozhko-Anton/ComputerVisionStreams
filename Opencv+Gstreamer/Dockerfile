FROM ubuntu:18.04
ENV PATH="/root/miniconda3/bin:${PATH}"
ARG PATH="/root/miniconda3/bin:${PATH}"

RUN apt-get update && apt-get install --no-install-recommends -y \
    apt-utils \
    build-essential \
    curl

RUN apt-get install --no-install-recommends -y \
    gstreamer1.0-plugins-ugly \
    gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-bad \
    gstreamer1.0-plugins-good \
    gstreamer1.0-x \
    gstreamer1.0-libav \
    gstreamer1.0-tools

RUN apt-get install --no-install-recommends -y \
    libgstreamer1.0-dev \
    libgstreamer-plugins-base1.0-dev \
    libglib2.0-dev
RUN apt-get install --no-install-recommends -y wget ca-certificates

RUN wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh -O ~/miniconda.sh
RUN bash ~/miniconda.sh -b -p $HOME/miniconda

RUN wget \
    https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh \
    && mkdir -p /root/.conda \
    && bash Miniconda3-latest-Linux-x86_64.sh -b \
    && rm -f Miniconda3-latest-Linux-x86_64.sh
RUN conda --version
RUN conda install jupyterlab notebook  -c conda-forge

WORKDIR /notebooks

RUN apt install -y git cmake
COPY install_opencv_with_gstreamer.sh /notebooks/
RUN ./install_opencv_with_gstreamer.sh
RUN pip install git+https://github.com/ytdl-org/youtube-dl.git@master#egg=youtube_dl
RUN pip install matplotlib

CMD ["/root/miniconda3/bin/jupyter","notebook", "--allow-root"]