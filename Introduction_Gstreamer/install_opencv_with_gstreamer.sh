OPENCV_VER="74"
TMPDIR=$(mktemp -d)

# Build and install OpenCV from source.
cd "${TMPDIR}"
git clone --branch ${OPENCV_VER} --depth 1 --recurse-submodules --shallow-submodules https://github.com/opencv/opencv-python.git opencv-python-${OPENCV_VER}
cd opencv-python-${OPENCV_VER}
export ENABLE_CONTRIB=0
export ENABLE_HEADLESS=1
# We want GStreamer support enabled.
export CMAKE_ARGS="-DWITH_GSTREAMER=ON"
python3 -m pip wheel . --verbose

# Install OpenCV
python3 -m pip install opencv_python*.whl