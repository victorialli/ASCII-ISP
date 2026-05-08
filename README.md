# ASCII-ISP

**ASCII-ISP** is a performance-oriented Software Image Signal Processor (ISP) written in C++. It transforms real-time video signals into ASCII-based visual data, optimized for terminal environments through fixed-point mathematics and adaptive spatial downsampling.

---

## ISP Pipeline Highlights

* **Fixed-Point Luma Mapping:** Implements a high-throughput integer approximation of the **ITU-R BT.601** standard ($$Y = (29R + 150G + 77B) >> 8$$) to map BGR signals to 8-bit luminance values without floating-point overhead.
* **Adaptive Spatial Downsampling:** Automatically probes terminal dimensions in real-time to calculate an optimal scaling factor. The engine compensates for the 2:1 height-to-width ratio of terminal characters, ensuring the video remains proportional and fits the display window perfectly.
* **8-bit Luma Quantization:** Maps 8-bit luminance values to a custom ASCII character ramp. Characters are selected and ordered based on their perceptual density (the ratio of "ink" to whitespace) to approximate a linear grayscale gradient, ensuring high-fidelity tonal reproduction in the terminal.
* **Zero-Allocation Frame Buffer:** Utilizes a "Write-Through" buffer strategy by pre-allocating memory and overwriting a persistent `std::string`, eliminating heap fragmentation and allocation latency during high-FPS playback.
* **Source-Synchronous Timing:** Dynamically tracks source metadata (FPS) to maintain temporal alignment between the decoded signal and the output display.

---

## Prerequisites

* **Compiler:** C++17 compatible.
* **Library:** [OpenCV 4.x](https://opencv.org/) (Signal decoding and capture).
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
* This project is intended for educational and technical demonstration purposes. All sample media remains the property of their respective creators and is used here under Public Domain and Creative Commons/Pexels guidelines.

---

## Preview

https://github.com/user-attachments/assets/d9ea5d99-86cb-4763-ad4f-781d34bb9963

<img width="2898" height="2027" alt="image" src="https://github.com/user-attachments/assets/70326393-f097-42a5-b1d7-cba31eb8b64d" />

