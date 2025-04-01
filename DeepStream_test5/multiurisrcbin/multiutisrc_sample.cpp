#include "pipeline.hpp"
#include <iostream>

using namespace deepstream;
#define CONFIG_FILE_PATH "/opt/nvidia/deepstream/deepstream/samples/configs/deepstream-app/config_infer_primary.yml"

class ObjectCounter : public BufferProbe::IBatchMetadataObserver
{
    public:
    ObjectCounter() {}
    virtual ~ObjectCounter() {}

    virtual probeReturn handleData(BufferProbe& probe, const BatchMetadata& data) {
        data.iterate([](const FrameMetadata& frame_data) {
        auto vehicle_count = 0;
        frame_data.iterate([&](const ObjectMetadata& object_data) {
            auto class_id = object_data.classId();
            if (class_id == 0) {
                vehicle_count++;
            }
        });
        std::cout << "Object Counter: " <<
            " Pad Idx = " << frame_data.padIndex() <<
            " Frame Number = " << frame_data.frameNum() <<
            " Vehicle Count = " << vehicle_count << std::endl;
        });

        return probeReturn::Probe_Ok;
    }
};

int main(int argc, char *argv[])
{
    try {
        Pipeline pipeline("sample-pipeline");
        pipeline
                //.add("nvurisrcbin", "src", "uri", argv[1])
                //.add("nvstreammux", "mux", "batch-size", 1, "width", 1280, "height", 720)
                .add("nvmultiurisrcbin", "src", 
                    "port", 9000, 
                    "ip-address", "localhost",
                    "drop-pipeline-eos", 1, // don't stop pipeline when there is no streams to process. wait
                    "max-batch-size", 8,
                    "batched-push-timeout", 33333,
                    "width", 1280, 
                    "height", 720)
                .add("nvinferbin", "infer", "config-file-path", CONFIG_FILE_PATH)
                .add("nvmultistreamtiler", "tiler")
                .add("nvdsosd", "osd")
                .add("nveglglessink", "sink")
                .link("src", "infer", "tiler", "osd", "sink");

        //pipeline.attach("infer", new BufferProbe("counter", new ObjectCounter));
        pipeline.start().wait();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    return 0;
}