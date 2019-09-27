/*
 * Copyright (C) 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#define LOG_TAG "android.hardware.copro@1.0-service.stm32mp1::CoproSerialPort"
// #define LOG_NDEBUG 0

#include <android-base/logging.h>
#include <assert.h>
#include <stdio.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "CoproSerialPort.h"

#define COPRO_TTYRMPSG_OPEN_TIMEOUT_S 4

namespace android {
namespace hardware {
namespace copro {
namespace V1_0 {
namespace implementation {

    CoproSerialPort::CoproSerialPort() {
        mFdRpmsg = -1;
    }

    CoproSerialPort::~CoproSerialPort() {
        if(mFdRpmsg == -1)
        {
            ::close(mFdRpmsg);
        }
    }

    Return<void> CoproSerialPort::open(TtyMode mode, ICoproSerialPortHal::open_cb _hidl_cb) {
        RequestStatus ret;
        struct termios tiorpmsg;
        int timeout = COPRO_TTYRMPSG_OPEN_TIMEOUT_S;

        ret.ret = false;

        ALOGV("Start CoproSerialPort::open %d", mode);

        if(mFdRpmsg == -1)
        {
            while ((mFdRpmsg = ::open(SERIAL_DEVICE_PATH, O_RDWR |  O_NOCTTY | O_NONBLOCK)) < 0) {
                sleep(1);
                timeout--;
                if (timeout == 0)
                    break;
                ALOGD("Failed to open, retry for %d s", timeout);
            }

            if (mFdRpmsg < 0) //Issue opening serial port
            {
                ALOGE("Error opening %s, err=-%d", SERIAL_DEVICE_PATH, errno);
                mFdRpmsg = -1;
                ret.ret = false;
            }
            else
            {
                ALOGV("configuring CoproSerialPort::open");
                tcgetattr(mFdRpmsg, &tiorpmsg);// get current port settings

                if (mode == TtyMode::MODE_RAW)
                {
                    ALOGV("CoproSerialPort::open mode RAW");
                    tiorpmsg.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
                                         | INLCR | IGNCR | ICRNL | IXON);
                    tiorpmsg.c_oflag &= ~OPOST;
                    tiorpmsg.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
                    tiorpmsg.c_cflag &= ~(CSIZE | PARENB);
                    tiorpmsg.c_cflag |= CS8;
                }
                else
                {
                    ALOGV("CoproSerialPort::open not mode RAW");
                    tiorpmsg.c_lflag &= ~ECHO;              // ECHO off, other bits unchanged
                    tiorpmsg.c_oflag &= ~ONLCR;             // do not convert LF to CR LF
                }

                tcsetattr(mFdRpmsg, TCSANOW, &tiorpmsg);
                ret.ret = true;
            }
        }

        ALOGV("End CoproSerialPort::open %d", mFdRpmsg);

        _hidl_cb(ret);

        return Void();
    }

    Return<void> CoproSerialPort::close(ICoproSerialPortHal::close_cb _hidl_cb) {
        RequestStatus ret;

        ret.ret = true;
        ALOGV("Start CoproSerialPort::close %d", mFdRpmsg);
        if(mFdRpmsg != -1)
        {
            if(::close(mFdRpmsg) < 0)
            {
                ret.ret = false;
            }
            else
            {
                mFdRpmsg = -1;
            }
        }
        ALOGV("End CoproSerialPort::close");
        _hidl_cb(ret);

        return Void();
    }

    Return<void> CoproSerialPort::read(ICoproSerialPortHal::read_cb _hidl_cb) {
        RequestStatus ret;
        std::string pData = "";
        int byte_rd, byte_avail;

        ret.ret = true;
        // ALOGV("Start CoproSerialPort::read %d", mFdRpmsg);

        if(mFdRpmsg == -1)
        {
            ALOGE("Error reading %s, fileDescriptor is not set", SERIAL_DEVICE_PATH);
            ret.ret = false;
        }
        else
        {
            ioctl(mFdRpmsg, FIONREAD, &byte_avail);

            // ALOGV("CoproSerialPort::read byte to read : %d", byte_avail);
            if (byte_avail > 0)
            {
                pData.resize(byte_avail);
                byte_rd = ::read (mFdRpmsg, &pData[0], byte_avail);
                ALOGI("readTtyRpmsg, read successfully %d bytes to %s", byte_rd, pData.c_str());
            }
        }

        // ALOGV("End CoproSerialPort::read");

        _hidl_cb(ret, pData);

        return Void();
    }

    Return<void> CoproSerialPort::write(const hidl_string& command, ICoproSerialPortHal::write_cb _hidl_cb) {
        RequestStatus ret;
        std::string pData = std::string(command);

        ret.ret = true;
        // ALOGV("Start CoproSerialPort::write %d", mFdRpmsg);
        if(mFdRpmsg == -1)
        {
            ALOGE("Error writing %s, fileDescriptor is not set", SERIAL_DEVICE_PATH);
            ret.ret = false;
        }
        else
        {

            // ALOGV("CoproSerialPort::write call write");
            if(::write(mFdRpmsg, pData.c_str(), pData.size()) <= 0)
            {
                ALOGE("Error writing [%s] on %s, fileDescriptor is not set", pData.c_str(), SERIAL_DEVICE_PATH);
                ret.ret = false;
            }
        }
        // ALOGV("End CoproSerialPort::write");

        _hidl_cb(ret);

        return Void();
    }

}  // namespace implementation
}  // namespace V1_0
}  // namespace copro
}  // namespace hardware
}  // namespace android
