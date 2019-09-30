#ifndef ANDROID_HARDWARE_COPRO_V1_0_COPROSERIALPROT_H
#define ANDROID_HARDWARE_COPRO_V1_0_COPROSERIALPROT_H

#include <android/hardware/copro/1.0/types.h>
#include <android/hardware/copro/1.0/ICoproSerialPortHal.h>
#include <hidl/Status.h>
#include <utils/Log.h>

namespace android {
namespace hardware {
namespace copro {
namespace V1_0 {
namespace implementation {

using ::android::hardware::copro::V1_0::TtyMode;
using ::android::hardware::copro::V1_0::RequestStatus;
using ::android::hidl::base::V1_0::DebugInfo;
using ::android::hidl::base::V1_0::IBase;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;

struct CoproSerialPort : public ICoproSerialPortHal {
    CoproSerialPort();
    ~CoproSerialPort();

    Return<void> open(TtyMode mode, open_cb _hidl_cb);
    Return<void> close(close_cb _hidl_cb);
    Return<void> read(read_cb _hidl_cb);
    Return<void> write(const hidl_string& command, write_cb _hidl_cb);
    Return<void> readB(const uint32_t size, readB_cb _hidl_cb);
    Return<void> writeB(const hidl_vec<uint8_t>& command, writeB_cb _hidl_cb);

    private:

    int mFdRpmsg;
    const char *SERIAL_DEVICE_PATH = "/dev/ttyRPMSG0";

};

}  // namespace implementation
}  // namespace V1_0
}  // namespace copro
}  // namespace hardware
}  // namespace android


#endif // ANDROID_HARDWARE_COPRO_V1_0_COPROSERIALPROT_H
