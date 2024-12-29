FROM nvcr.io/nvidia/jps/vlm_summarization:2.0.8
RUN apt install python3-pip && \
    python3 -m pip uninstall openai -y && \
    python3 -m pip cache purge && \
    python3 -m pip install openai
