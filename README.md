xqqyuv
======
Simple YUV-RGB conversion library with SIMD accelerated, still under developing and just for fun.

For industry use, please consider [libyuv](https://code.google.com/p/libyuv/).

### Features
- SSE2 accelerated conversion in I420ToARGB

### TODO
- gcc support (makefile / AT&T inline-asm)

- x64 support (standalone .asm file)

- AVX2/FMA support (My CPU doesn't support these instructions yet...)

### NOT_ON_PLAN
- Other platforms: e.g. ARM NEON

### xqqyuv_player
- Simple demo for xqqyuv. It can play YUV file which is outputed from vlc.

- Render: SDL v1.2

- Usage: xqqyuv_player [FILE] [FRAMERATE]

### License
Copyright (C) 2014 zheng qian <xqq@0ginr.com>

Licensed under the [WTFPL](http://en.wikipedia.org/wiki/WTFPL), Version 2.0 (the "License");