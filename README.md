# ASCII-ISP

**ASCII-ISP** is an ISP-inspired C++ terminal ASCII renderer for images and video. It currently focuses on fixed-point luma conversion, terminal-aware downsampling, and ASCII quantization, with additional ISP-style preprocessing stages planned for future development.

---

## Current ISP-Inspired Pipeline

* **Fixed-Point Luma Mapping:** Uses an integer approximation of **ITU-R BT.601** luma conversion (`Y = (77R + 150G + 29B) >> 8`), avoiding floating-point work during per-pixel processing.
* **Terminal-Aware Downsampling:** Computes a scale factor from the terminal dimensions and compensates for the approximate 2:1 height-to-width ratio of terminal characters so the rendered output fits the screen proportionally.
* **8-bit Luma Quantization:** Maps luminance values to a custom ASCII character ramp chosen to approximate a grayscale gradient in a monospaced terminal.
* **Reusable Output Buffer:** Reuses a reserved `std::string` for frame output to reduce repeated allocations during playback.
* **FPS-Based Playback Timing:** Uses source FPS metadata to pace video frame output in the terminal.

---

## Prerequisites

* **Compiler:** C++17 compatible.
* **Library:** [OpenCV 4.x](https://opencv.org/) (image/video decoding and frame access).
* **Build System:** [CMake](https://cmake.org/) (3.15+).

---

## Build with CMake 

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

---

## Usage

### On Windows

```bash
.\Release\ascii_isp.exe ..\samples\Earth_View_shuttle_over_clouds.webmhd.webm
```

### On macOS/Linux

```bash
./ascii_isp ../samples/Earth_View_shuttle_over_clouds.webmhd.webm
```

---

## Credits

### Assets

* `Earth_View_shuttle_over_clouds.webmhd.webm` is courtesy of the NASA Scientific Visualization Studio. ([Source](https://svs.gsfc.nasa.gov/10766/#media_group_351924))
* `KSC-20260428-PH-FMX01_0104~small.jpg` is courtesy of the NASA Image and Video Library. ([Source](https://images-assets.nasa.gov/image/KSC-20260428-PH-FMX01_0104/KSC-20260428-PH-FMX01_0104~small.jpg))
* `13582958_1080_1920_50fps.mp4` is provided via Pexels under their free license. ([Source]( https://www.pexels.com/video/dynamic-modern-dance-performance-in-studio-31889245/))

### Disclosures

* Generative AI was utilized to implement the Win32 API console abstractions for Windows compatibility and to assist in the drafting of this technical documentation.
* This project is intended as an educational and technical demonstration. All sample media remains the property of their respective creators and is used here under Public Domain and Creative Commons/Pexels guidelines.

---

## Preview

https://github.com/user-attachments/assets/d9ea5d99-86cb-4763-ad4f-781d34bb9963

<img width="2898" height="2027" alt="image" src="https://github.com/user-attachments/assets/70326393-f097-42a5-b1d7-cba31eb8b64d" />

