PRLib - Pre-Recognition Library.

The main aim of the library - prepare image for recogntion. Image processing can really help to improve recognition quality.

*Features:*
* Binarization (Global Otsu, Sauvola, Niblack, Adaptive)
* Deskew
* Denoise (Non-local Means Denoising algorithm)
* Thinning (Zhang-Suen, Guo-Hall)
* Blur detection (LAPM, LAPV, TENG, GLVN)
* Deblur (Gaussian-based)
* White balance (Simple, Grayworld)
* Border detection (Canny-based)
* Removing perspective warp
* Crop

*Dependencies:* OpenCV + OpenCV contrib, Leptonica.

*How to build:*
1) Go to the directory with source code
2) Make directory for build:
```
    mkdir build
```
3) Run cmake:
```
    cmake ..
```    
4) Run make:
```
    make
```    

*Build tested*

- Opencv: 4.7.0
- Leptonica: 1.82.0-3
