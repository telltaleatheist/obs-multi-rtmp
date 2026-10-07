#pragma once

#include "pch.h"
#include "output-config.h"

#include <functional>
#include <string>

// Stream profiles: named sets of targets, picked at the top of the dock.
// Picking one shows only its targets, and "Start all" / "Stop all" act on
// exactly those, so one click starts e.g. Twitch + YouTube for a given show.
class StreamProfileBar : public QWidget {
public:
    StreamProfileBar(QWidget* parent, std::function<void()> onActiveChanged);

    // Rebuild the picker from the current config (after a config load).
    void Reload();

    // Whether a target belongs to the active profile (always true with no profile active).
    static bool TargetInActiveProfile(const std::string& targetId);
    // A new target joins the active profile, so it shows up where it was created.
    static void OnTargetAdded(const std::string& targetId);
    // A deleted target leaves every profile.
    static void OnTargetDeleted(const std::string& targetId);

private:
    QComboBox* picker_;
    QPushButton* editBtn_;
    QPushButton* deleteBtn_;
    std::function<void()> onActiveChanged_;
    bool reloading_ = false;

    void Select(const std::optional<std::string>& profileId);
    void UpdateButtons();
    bool EditProfile(StreamProfileConfig& profile, bool isNew);
};
