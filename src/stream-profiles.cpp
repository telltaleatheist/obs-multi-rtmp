#include "stream-profiles.h"

#include <algorithm>

#include <QDialog>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "output-config.h"

static StreamProfileConfigPtr ActiveProfile()
{
    auto& config = GlobalMultiOutputConfig();
    if (!config.activeProfile.has_value())
        return nullptr;
    return FindById(config.profiles, *config.activeProfile);
}

bool StreamProfileBar::TargetInActiveProfile(const std::string& targetId)
{
    auto profile = ActiveProfile();
    if (!profile)
        return true;
    auto& ids = profile->targetIds;
    return std::find(ids.begin(), ids.end(), targetId) != ids.end();
}

void StreamProfileBar::OnTargetAdded(const std::string& targetId)
{
    if (auto profile = ActiveProfile())
        profile->targetIds.push_back(targetId);
}

void StreamProfileBar::OnTargetDeleted(const std::string& targetId)
{
    for (auto& profile : GlobalMultiOutputConfig().profiles) {
        auto& ids = profile->targetIds;
        ids.erase(std::remove(ids.begin(), ids.end(), targetId), ids.end());
    }
}

StreamProfileBar::StreamProfileBar(QWidget* parent, std::function<void()> onActiveChanged)
    : QWidget(parent), onActiveChanged_(std::move(onActiveChanged))
{
    auto layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(new QLabel(obs_module_text("Profile.Label"), this));
    picker_ = new QComboBox(this);
    picker_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    layout->addWidget(picker_, 1);

    auto newBtn = new QPushButton(obs_module_text("Profile.New"), this);
    editBtn_ = new QPushButton(obs_module_text("Btn.Edit"), this);
    deleteBtn_ = new QPushButton(obs_module_text("Btn.Delete"), this);
    layout->addWidget(newBtn);
    layout->addWidget(editBtn_);
    layout->addWidget(deleteBtn_);

    QObject::connect(picker_, &QComboBox::currentIndexChanged, [this](int index) {
        if (reloading_ || index < 0)
            return;
        auto id = picker_->itemData(index).toString().toStdString();
        auto& config = GlobalMultiOutputConfig();
        if (id.empty())
            config.activeProfile.reset();
        else
            config.activeProfile = id;
        SaveMultiOutputConfig();
        UpdateButtons();
        onActiveChanged_();
    });

    QObject::connect(newBtn, &QPushButton::clicked, [this]() {
        auto& config = GlobalMultiOutputConfig();
        auto profile = std::make_shared<StreamProfileConfig>();
        profile->id = GenerateId(config);
        if (!EditProfile(*profile, true))
            return;
        config.profiles.push_back(profile);
        config.activeProfile = profile->id;
        SaveMultiOutputConfig();
        Reload();
        onActiveChanged_();
    });

    QObject::connect(editBtn_, &QPushButton::clicked, [this]() {
        auto profile = ActiveProfile();
        if (!profile || !EditProfile(*profile, false))
            return;
        SaveMultiOutputConfig();
        Reload();
        onActiveChanged_();
    });

    QObject::connect(deleteBtn_, &QPushButton::clicked, [this]() {
        auto profile = ActiveProfile();
        if (!profile)
            return;
        QMessageBox box(QMessageBox::Icon::Question, obs_module_text("Question.Title"),
                        QString(obs_module_text("Profile.DeleteQuestion")).arg(QString::fromStdString(profile->name)),
                        QMessageBox::Yes | QMessageBox::No, this);
        if (box.exec() != QMessageBox::Yes)
            return;
        auto& config = GlobalMultiOutputConfig();
        config.profiles.remove(profile);
        config.activeProfile.reset();
        SaveMultiOutputConfig();
        Reload();
        onActiveChanged_();
    });

    Reload();
}

void StreamProfileBar::Reload()
{
    reloading_ = true;
    picker_->clear();
    picker_->addItem(obs_module_text("Profile.All"), QString());
    auto& config = GlobalMultiOutputConfig();
    for (auto& profile : config.profiles)
        picker_->addItem(QString::fromStdString(profile->name), QString::fromStdString(profile->id));

    // A saved active profile that no longer exists means "all targets".
    if (config.activeProfile.has_value() && !ActiveProfile())
        config.activeProfile.reset();
    Select(config.activeProfile);
    reloading_ = false;
    UpdateButtons();
}

void StreamProfileBar::Select(const std::optional<std::string>& profileId)
{
    auto wanted = QString::fromStdString(profileId.value_or(""));
    for (int i = 0; i < picker_->count(); ++i) {
        if (picker_->itemData(i).toString() == wanted) {
            picker_->setCurrentIndex(i);
            return;
        }
    }
}

void StreamProfileBar::UpdateButtons()
{
    bool hasProfile = ActiveProfile() != nullptr;
    editBtn_->setEnabled(hasProfile);
    deleteBtn_->setEnabled(hasProfile);
}

bool StreamProfileBar::EditProfile(StreamProfileConfig& profile, bool isNew)
{
    QDialog dlg(this);
    dlg.setWindowTitle(obs_module_text(isNew ? "Profile.NewTitle" : "Profile.EditTitle"));
    auto layout = new QVBoxLayout(&dlg);

    layout->addWidget(new QLabel(obs_module_text("Profile.Name"), &dlg));
    auto name = new QLineEdit(QString::fromStdString(profile.name), &dlg);
    layout->addWidget(name);

    layout->addWidget(new QLabel(obs_module_text("Profile.Targets"), &dlg));
    auto list = new QListWidget(&dlg);
    for (auto& target : GlobalMultiOutputConfig().targets) {
        auto item = new QListWidgetItem(QString::fromStdString(target->name), list);
        item->setData(Qt::UserRole, QString::fromStdString(target->id));
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        bool member = std::find(profile.targetIds.begin(), profile.targetIds.end(), target->id) != profile.targetIds.end();
        item->setCheckState(member ? Qt::Checked : Qt::Unchecked);
    }
    layout->addWidget(list);

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    QObject::connect(buttons, &QDialogButtonBox::accepted, &dlg, [&]() {
        if (name->text().trimmed().isEmpty()) {
            QMessageBox::warning(&dlg, obs_module_text("Notice.Title"), obs_module_text("Profile.NeedName"));
            return;
        }
        dlg.accept();
    });
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    layout->addWidget(buttons);
    dlg.setMinimumWidth(360);

    if (dlg.exec() != QDialog::Accepted)
        return false;

    profile.name = tostdu8(name->text().trimmed());
    profile.targetIds.clear();
    for (int i = 0; i < list->count(); ++i) {
        auto item = list->item(i);
        if (item->checkState() == Qt::Checked)
            profile.targetIds.push_back(item->data(Qt::UserRole).toString().toStdString());
    }
    return true;
}
