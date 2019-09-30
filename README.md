# hardware-copro #

This module contains the STMicroelectronics Copro HAL source code.
It is part of the STMicroelectronics Android delivery (see the [delivery][] for more information).

[delivery]: https://wiki.st.com/stm32mpu/wiki/STM32MP15_distribution_for_Android_release_note_-_v1.1.0

## Description ##

This module version includes the first version of the Copro Android abstraction layer.
It is based on the proprietary Copro API.

Please see the Android delivery release notes for more details.

## Documentation ##

* The [release notes][] provide information on the release.
* The [distribution package][] provides detailed information on how to use this delivery.

[release notes]: https://wiki.st.com/stm32mpu/wiki/STM32MP15_distribution_for_Android_release_note_-_v1.1.0
[distribution package]: https://wiki.st.com/stm32mpu/wiki/STM32MP1_Distribution_Package_for_Android

## Dependencies ##

This module can't be used alone. It is part of the STMicroelectronics Android delivery.
To be able to use it the device.mk must have the following packages:
```
PRODUCT_PACKAGES += \
    copro.stm \
    android.hardware.copro@<version>-service.stm32mp1 \
    CoproService
```

## Containing ##

This directory contains the sources and associated Android makefile to generate the copro.stm library.

## License ##

This module is distributed under the Apache License, Version 2.0 found in the [LICENSE](./LICENSE) file.
