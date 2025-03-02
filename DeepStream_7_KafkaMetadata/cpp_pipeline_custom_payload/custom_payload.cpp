#include "pipeline.hpp"
#include <iostream>
using namespace deepstream;
#define CONFIG_FILE_PATH "/opt/nvidia/deepstream/deepstream/samples/configs/deepstream-app/config_infer_primary.yml"


int main(int argc, char *argv[])
{
    try {
        Pipeline pipeline("sample-pipeline");
        pipeline.add("nvurisrcbin", "src", 
                     "uri", argv[1])
                .add("nvstreammux", "mux", 
                     "batch-size", 1, 
                     "width", 1280, 
                     "height", 720)
                .add("nvinferbin", "infer", 
                    "config-file-path", CONFIG_FILE_PATH)
                .add("nvosdbin", "osd")
                .add("nvmsgconv", "msgconv", 
                     "config", "/workspace/cpp_pipeline/cfg_msgconv.txt", // schema : sensors, places, modules
                     //"msg2p-lib", "libnvds_msgconv_custom.so",
                     "payload-type", 257, // PAYLOAD_DEEPSTREAM, PAYLOAD_DEEPSTREAM_MINIMAL, PAYLOAD_CUSTOM (257)
                     "comp-id", 2, // comp-id=2 Default is NvDsEventMsgMeta
                     "debug-payload-dir", "./debug-payload", // default NULL
                     "msg2p-newapi", 1, // Generate payloads using Gst buffer frame/object metadata (default 0)
                     "frame-interval", 1 // default 30
                     )
                .add("nvmsgbroker", "brokersink", 
                     //"config", NVMSGBROKER_CONFIG,
                     "conn-str", "localhost;9092", // <kafka broker address>;<port>
                     "proto-lib", "/opt/nvidia/deepstream/deepstream-7.0/lib/libnvds_kafka_proto.so",
                     "comp-id", 2,
                     "topic", "test",
                     "new-api", 1,
                     "sleep-time", 10 // sleep time in ms for do_work
                     )
                .link({"src", "mux"}, {"", "sink_%u"}).link("mux","infer", "osd", "msgconv", "brokersink");
                // simple sink
                // .add("nveglglessink", "sink")
                // .link({"src", "mux"}, {"", "sink_%u"}).link("mux","infer", "osd", "sink");

        //pipeline.attach("infer", new BufferProbe("NvDsPayloadProbe", new NvDsPayloadProbe));
        pipeline.start().wait();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    return 0;
}