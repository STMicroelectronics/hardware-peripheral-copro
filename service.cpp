/*
 * Copyright (C) 2016 The Android Open Source Project
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

#define LOG_TAG "android.hardware.copro@1.0-service.stm32mp1"

#define LOG_NDEBUG 0

#include <hidl/HidlTransportSupport.h>
#include "Copro.h"
#include "CoproSerialPort.h"

using android::sp;

// libhwbinder:
using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;

// Generated HIDL files
using android::hardware::copro::V1_0::ICopro;
using android::hardware::copro::V1_0::ICoproSerialPortHal;
using ::android::hardware::copro::V1_0::implementation::Copro;
using ::android::hardware::copro::V1_0::implementation::CoproSerialPort;

using android::OK;
using android::status_t;

int main() {
  android::sp<ICopro> service = new Copro();
  android::sp<ICoproSerialPortHal> serviceSerialPort = new CoproSerialPort();

  configureRpcThreadpool(1, true /*callerWillJoin*/);
  status_t status = service->registerAsService();

  if (status != OK) {
    ALOGE("Cannot register COPRO HAL service");
    return 1;
  }

  status = serviceSerialPort->registerAsService();
  if (status != OK) {
    ALOGE("Cannot register COPRO HAL serviceSerialPort");
    return 1;
  }

  ALOGI("COPRO HAL Ready.");
  joinRpcThreadpool();
  // Under noraml cases, execution will not reach this line.
  ALOGI("COPRO HAL failed to join thread pool.");
  return 1;
}
