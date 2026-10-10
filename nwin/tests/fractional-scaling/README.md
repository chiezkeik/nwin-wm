<!--
    SPDX-FileCopyrightText: None

    SPDX-License-Identifier: CC0-1.0
-->

# Fractional scaling

### How to build and run it

```
cmake -B build
cmake --build build --parallel
build/bin/fractionalscalingtest
```

Note that you also need to start `nwin_wayland` with the `NWIN_WAYLAND_SUPPORT_XX_FRACTIONAL_SCALE_V2=1` environment variable set.
