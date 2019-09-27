#ifndef ANDROID_HARDWARE_COPRO_V1_0_COPRO_H
#define ANDROID_HARDWARE_COPRO_V1_0_COPRO_H

#include <android/hardware/copro/1.0/ICopro.h>
#include <android/hardware/copro/1.0/types.h>
#include <hidl/Status.h>
#include <utils/Log.h>

namespace android {
namespace hardware {
namespace copro {
namespace V1_0 {
namespace implementation {

using ::android::hardware::copro::V1_0::HalFwInfo;
using ::android::hardware::copro::V1_0::RequestStatus;
using ::android::hidl::base::V1_0::DebugInfo;
using ::android::hidl::base::V1_0::IBase;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;

struct Copro : public ICopro {
    Copro();

    Return<void> getFwList(getFwList_cb _hidl_cb) override;
    Return<void> getFwByName(const hidl_string& name, getFwByName_cb _hidl_cb) override;
    Return<void> isFwRunning(uint32_t id, isFwRunning_cb _hidl_cb) override;
    Return<void> startFw(uint32_t id, startFw_cb _hidl_cb) override;
    Return<void> stopFw(uint32_t id, stopFw_cb _hidl_cb) override;

private:
    const std::string FIRMWARE_PREFIX_PATH = "copro/";
    const std::string FIRMWARE_PATH = "/vendor/firmware/copro/";
    const unsigned int MAX_BUFFER_SIZE = 80;

    std::vector<HalFwInfo> mFwList;
    uint32_t mIdCurrentRunningFw;

    bool setFirmwarePath(std::string path);
    void initFirmwareList();
    std::string getCurrentFirmware();
    bool getCurrentFirmwareState();
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace copro
}  // namespace hardware
}  // namespace android


#endif // ANDROID_HARDWARE_COPRO_V1_0_COPRO_H
