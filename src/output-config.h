#pragma once

#include <string>
#include <optional>
#include <vector>
#include <unordered_map>
#include <memory>
#include <list>

#include <json.hpp>

struct VideoEncoderConfig {
    std::string id;
    std::string encoderId;
    int fpsDenumerator = 1;
    nlohmann::json encoderParams;
    std::optional<std::string> outputScene;
    std::optional<std::string> resolution;
};
using VideoEncoderConfigPtr = std::shared_ptr<VideoEncoderConfig>;

struct AudioTrackConfig {
    int mixer_track;
    int output_track;
};
using AudioTrackConfigPtr = std::shared_ptr<AudioTrackConfig>;

struct AudioEncoderConfig {
    std::string id;
    std::string encoderId;
    nlohmann::json encoderParams;
    int mixerId = 0;
    std::list<AudioTrackConfigPtr> audioTracks;
};
using AudioEncoderConfigPtr = std::shared_ptr<AudioEncoderConfig>; 


struct OutputTargetConfig {
    std::string id;
    std::string name;
    std::string protocol = "RTMP";
    bool syncStart = false;
    bool syncStop = false;

    nlohmann::json serviceParam;
    nlohmann::json outputParam;

    std::optional<std::string> videoConfig;
    std::optional<std::string> audioConfig;
};
using OutputTargetConfigPtr = std::shared_ptr<OutputTargetConfig>;


// A named set of targets started and stopped together, like a vMix streaming
// preset. Targets are stored once and may belong to any number of profiles.
struct StreamProfileConfig {
    std::string id;
    std::string name;
    std::vector<std::string> targetIds;
};
using StreamProfileConfigPtr = std::shared_ptr<StreamProfileConfig>;


struct MultiOutputConfig {
public:
    std::list<OutputTargetConfigPtr> targets;
    std::list<VideoEncoderConfigPtr> videoConfig;
    std::list<AudioEncoderConfigPtr> audioConfig;
    std::list<StreamProfileConfigPtr> profiles;
    std::optional<std::string> activeProfile; // empty = show every target
};

template<class T, class S>
inline T FindById(std::list<T>& list, const S& id) {
    for(auto& x: list) {
        if (x->id == id)
            return x;
    }
    return nullptr;
}


MultiOutputConfig& GlobalMultiOutputConfig();

void SaveMultiOutputConfig();

bool LoadMultiOutputConfig();

std::string GenerateId(MultiOutputConfig& config);
