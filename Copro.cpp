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
#define LOG_TAG "android.hardware.copro@1.0-service.stm32mp1::Copro"
// #define LOG_NDEBUG 0

#include <android-base/logging.h>
#include <assert.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <dirent.h>

#include "Copro.h"

namespace android {
namespace hardware {
namespace copro {
namespace V1_0 {
namespace implementation {

    Copro::Copro() {
        mIdCurrentRunningFw = 0;
    }

    Return<void> Copro::getFwList(ICopro::getFwList_cb _hidl_cb){
        RequestStatus ret;
        ALOGV("COPRO HAL : entering getFwList");

        initFirmwareList();
        ret.ret = true;

        _hidl_cb(ret, mFwList);

        ALOGV("COPRO HAL : end getFwList");

        return Void();
    }

    Return<void> Copro::getFwByName(const ::android::hardware::hidl_string& name, ICopro::getFwByName_cb _hidl_cb) {
        RequestStatus ret;
        HalFwInfo fw;

        ret.ret = false;

        initFirmwareList();

        for (auto fwInList = mFwList.begin(); fwInList != mFwList.end(); ++fwInList)
        {
            if (fwInList->name == name)
            {
                fw.id = fwInList->id;
                fw.name = fwInList->name;
                fw.state = fwInList->state;
                ret.ret = true;
                break;
            }
        }

        _hidl_cb(ret, fw);

        return Void();
    }

    Return<void> Copro::isFwRunning(uint32_t id, ICopro::isFwRunning_cb _hidl_cb) {
        RequestStatus ret;
        bool val = false;

        ret.ret = false;
        ALOGV("COPRO HAL : isFwRunning");
        if(id < mFwList.size())
        {
            val = mFwList[id].state;
            ret.ret = true;
        }
        ALOGV("COPRO HAL : end isFwRunning");
        _hidl_cb(ret, val);

        return Void();
    }

    Return<void> Copro::startFw(uint32_t id, ICopro::startFw_cb _hidl_cb) {
        RequestStatus ret;
        int fd = -1;
        bool isCurrentFwRunning = getCurrentFirmwareState();
        ret.ret = false;
        ALOGV("COPRO HAL : entering startFw %d", id);

        if(id < mFwList.size() && !isCurrentFwRunning) //Providing starting a fw if one is already running
        {
            ALOGD("COPRO HAL : startFw name %s", mFwList[id].name.c_str());
            if(setFirmwarePath(mFwList[id].name))
            {
                fd = open("/sys/class/remoteproc/remoteproc0/state", O_RDWR);
                if (fd < 0)
                {
                    ALOGE("Error opening remoteproc0/state, fw_id=%d, fw_name=%s, err=-%d", id, mFwList[id].name.c_str(), errno);
                }
                else
                {
                    write(fd, "start", strlen("start"));
                    close(fd);
                    mFwList[id].state = true;
                    mIdCurrentRunningFw = id;
                    ret.ret = true;
                }
            }
        }
        ALOGV("COPRO HAL : end startFw");

        _hidl_cb(ret);

        return Void();
    }

    Return<void> Copro::stopFw(uint32_t id __unused, ICopro::stopFw_cb _hidl_cb) {
        int fd;
        RequestStatus ret;
        ret.ret = false;
        ALOGV("COPRO HAL : entering stopFw %d", mIdCurrentRunningFw);

        fd = open("/sys/class/remoteproc/remoteproc0/state", O_RDWR);
        if (fd < 0)
        {
            ALOGE("Error opening remoteproc0/state, fw_id=%d, fw_name=%s, err=-%d", mIdCurrentRunningFw, mFwList[mIdCurrentRunningFw].name.c_str(), errno);
        }
        else
        {
            write(fd, "stop", strlen("stop"));
            close(fd);
            mFwList[mIdCurrentRunningFw].state = false;
            mIdCurrentRunningFw = mFwList.size();
            ret.ret = true;
        }

        ALOGV("COPRO HAL : end stopFw");

        _hidl_cb(ret);

        return Void();
    }


    bool Copro::setFirmwarePath(std::string fileName) {
        int fd;
        int result = -1;
        bool isCurrentFwRunning = getCurrentFirmwareState();
        std::string path = FIRMWARE_PREFIX_PATH + fileName;

        ALOGV("COPRO HAL : entering setFirmwarePath");

        if(!isCurrentFwRunning)
        {
            ALOGD("COPRO HAL : setFirmwarePath %s", path.c_str());

            fd = open("/sys/class/remoteproc/remoteproc0/firmware", O_RDWR);

            if (fd < 0)
            {
                ALOGE("Error opening remoteproc0/firmware, err=-%d", errno);
                return (errno * -1);
            }

            result = write(fd, path.c_str(), path.size());
            close(fd);
        }

        ALOGV("COPRO HAL : end setFirmwarePath");
        return (result > 0);
    }


    void Copro::initFirmwareList() {
        HalFwInfo tmp;
        struct dirent *entry;
        std::string currentFirmwarePath = getCurrentFirmware();
        bool currentFirmwareState = getCurrentFirmwareState();

        mFwList.clear(); //Clear the firmware list
        mIdCurrentRunningFw = 0;

        DIR *dir = opendir(FIRMWARE_PATH.c_str());
        if (dir != NULL)
        {
            ALOGV("COPRO HAL : entering initFirmwareList (%s, %d)", currentFirmwarePath.c_str(), currentFirmwareState);
            while ((entry = readdir(dir)) != NULL)
            {
                if(entry-> d_type == DT_REG)
                {
                    ALOGD("COPRO HAL : entry - %s ", entry->d_name);
                    tmp.state = false;
                    tmp.id = mFwList.size();
                    tmp.name = entry->d_name;

                    if(currentFirmwarePath.compare(FIRMWARE_PREFIX_PATH + std::string(entry->d_name)) == 0 && currentFirmwareState == true) 
                    {
                        tmp.state = true;
                        mIdCurrentRunningFw = tmp.id;
                        ALOGD("COPRO HAL : state - %s already running", entry->d_name);
                    }

                    mFwList.push_back(tmp);
                }
            }

            if(mIdCurrentRunningFw != 0)
            {
               mIdCurrentRunningFw = mFwList.size();
            }

            closedir(dir);
            ALOGV("COPRO HAL : initFirmwareList end ");
        }
    }


    std::string Copro::getCurrentFirmware() {
        int fd;
        ssize_t byte_read;
        char bufRead[MAX_BUFFER_SIZE];

        fd = open("/sys/class/remoteproc/remoteproc0/firmware", O_RDWR);

        if (fd < 0)
        {
            ALOGE("Error opening remoteproc0/firmware, err=-%d", errno);
            return "";
        }

        byte_read = read (fd, bufRead, MAX_BUFFER_SIZE);
        close(fd);

        if (byte_read != -1 )
        {
            return std::string(bufRead, byte_read-1); //To remove \n at the end of string
        }

        return "";
    }

    bool Copro::getCurrentFirmwareState() {
        int fd;
        ssize_t byte_read;
        char bufRead[MAX_BUFFER_SIZE];

        fd = open("/sys/class/remoteproc/remoteproc0/state", O_RDWR);

        if (fd < 0)
        {
            ALOGE("Error opening remoteproc0/state, err=-%d", errno);
            return false;
        }

        byte_read = read (fd, bufRead, MAX_BUFFER_SIZE);
        close(fd);
        if (byte_read != -1 )
        {
            return (std::string(bufRead, byte_read-1).compare("running") == 0);
        }
        return false;
    }

}  // namespace implementation
}  // namespace V1_0
}  // namespace copro
}  // namespace hardware
}  // namespace android

