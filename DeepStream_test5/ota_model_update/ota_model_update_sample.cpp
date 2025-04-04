#include "pipeline.hpp"
#include "lib/model_engine_watch_otf_trigger.hpp"
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
                .add("nvurisrcbin", "src", "uri", argv[1], "file-loop", 1)
                .add("nvstreammux", "mux", "batch-size", 1, "width", 1280, "height", 720)
                .add("nvinferbin", "infer", "config-file-path", CONFIG_FILE_PATH)
                .add("nvmultistreamtiler", "tiler")
                .add("nvdsosd", "osd")
                .add("nveglglessink", "sink")
                .link({"src", "mux"}, {"", "sink_%u"}).link("mux","infer", "tiler", "osd", "sink");

        pipeline.attach("infer", new BufferProbe("counter", new ObjectCounter));

        std::vector<std::unique_ptr<NvDsModelEngineWatchOTFTrigger>> otf_triggers;
        std::string model_engine_file;
        std::string pgie = "infer";
        pipeline[pgie].getProperty("model-engine-file", model_engine_file); 
        // create model update watcher
        otf_triggers.push_back(std::make_unique<NvDsModelEngineWatchOTFTrigger>(&pipeline[pgie], model_engine_file));
        NvDsModelEngineWatchOTFTrigger& otf_trigger = *otf_triggers.back().get();
  
        // /opt/nvidia/deepstream/deepstream/samples/configs/deepstream-app/config_infer_primary.yml
        // model-engine-file: ../../models/Primary_Detector/resnet18_trafficcamnet.etlt_b30_gpu0_int8.engine
         pipeline.start([&otf_trigger](Pipeline &p, const Pipeline::Message &msg) {
          const Pipeline::StateTransitionMessage *state_change_msg = dynamic_cast<const Pipeline::StateTransitionMessage*>(&msg);
          // handle state change message
          if (state_change_msg) {
            static bool otf_trigger_started = false;
            if (!otf_trigger_started) {
              otf_trigger.start();
              otf_trigger_started = true;
            }
          }
        }).wait();

        // pipeline.start().wait();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    return 0;
}