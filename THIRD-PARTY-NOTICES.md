# Third-Party Notices

This repository contains code that is derived from third-party software. The
original license terms are reproduced below.

## Zumo32U4Encoders (Pololu Corporation)

- **Source:** https://github.com/pololu/zumo-32u4-arduino-library
  (file `Zumo32U4Encoders`, i.e. `Zumo32U4Encoders.cpp` and `Zumo32U4Encoders.h`)
- **Used in:** `library/Zumo328PEncoders.cpp` and `library/Zumo328PEncoders.h`
- **Relationship:** derived from the Zumo32U4Encoders class of Pololu's
  Zumo 32U4 Arduino library. The class interface, parts of the source structure
  and the documentation comments originate from that code. The interrupt
  handling, pin assignment and counter type were changed for this library.
- **License:** MIT License (see below)

```
Copyright (c) 2015-2022 Pololu Corporation (www.pololu.com)

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```
