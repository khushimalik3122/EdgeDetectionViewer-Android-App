# 📱 EdgeDetectionViewer-Android-App  
*A Real-Time Edge Detection Viewer using Android Camera2 API, OpenCV, and OpenGL ES*

> 👩‍💻 Built as part of a research internship to explore real-time image processing pipelines on Android.

---

## 🚀 Overview

EdgeDetectionViewer is an advanced Android application that performs **real-time edge detection** by combining:

- **Android Camera2 API** – Live camera feed  
- **OpenCV (C++ via JNI)** – Canny edge detection  
- **OpenGL ES 2.0+** – Render processed output

---

## ✨ Features

### ✅ Core Features
- 🔁 Live Camera Feed (Camera2 + TextureView)  
- 🧬 OpenCV Canny Edge Detection via JNI  
- 🎨 OpenGL ES Texture Rendering  
- ⚡ Smooth Performance (10–15 FPS)

### 🔍 Bonus Features
- Toggle raw vs processed view  
- Live FPS counter  
- Extra filters in C++ (not exposed in UI yet)  
- Shader pipeline (basic OpenGL fragment/vertex)

---

## 🧠 Architecture

```
/app
├── src/
│   ├── main/
│   │   ├── java/com/edgedetection/viewer/
│   │   │   └── MainActivity.kt
│   │   └── cpp/
│   │       ├── native-lib.cpp
│   │       ├── EdgeDetector.cpp/.h
│   │       └── GLRenderer.cpp/.h
└── res/
```

---

## 🔄 Frame Pipeline

1. 📸 Camera2 API captures frames to TextureView  
2. 🔁 JNI transfers frame data to native C++  
3. 🧪 OpenCV applies Canny edge detection  
4. 🎨 OpenGL ES renders frame as texture  
5. 📲 Output shown in real-time with FPS display  

---

## 🛠 Setup Instructions

### ✅ Requirements
- Android Studio 2021.2.1+  
- Android NDK 25.0+  
- CMake 3.18.1+  
- OpenCV 4.6.0 Android SDK

### 📦 Installation

1. **Clone the Repository**
```bash
git clone https://github.com/your-username/EdgeDetectionViewer-Android-App.git
cd EdgeDetectionViewer-Android-App
```

2. **Download OpenCV Android SDK**  
   [OpenCV Download Page](https://opencv.org/releases/)

3. **Update SDK Path** in `gradle.properties`
```properties
opencvsdk=/absolute/path/to/OpenCV-android-sdk
```

4. **Build and Run**  
   - Open project in Android Studio  
   - Sync Gradle  
   - Connect Android device (API 21+)  
   - Run 🚀

---

## 🔐 Permissions

Requests camera permission at runtime.

---

## ⚙️ CMake Configuration

```cmake
include_directories(${OpenCV_DIR}/sdk/native/jni/include)

add_library(lib_opencv SHARED IMPORTED)
set_target_properties(lib_opencv PROPERTIES
    IMPORTED_LOCATION ${OpenCV_DIR}/sdk/native/libs/${ANDROID_ABI}/libopencv_java4.so)

target_link_libraries(native-lib
    lib_opencv
    ${log-lib}
    ${android-lib}
    ${gles-lib}
    ${egl-lib})
```

---

## 🧱 Gradle Configuration

```gradle
externalNativeBuild {
    cmake {
        cppFlags "-frtti -fexceptions"
        abiFilters 'x86', 'x86_64', 'armeabi-v7a', 'arm64-v8a'
        arguments "-DOpenCV_DIR=" + opencvsdk + "/sdk/native"
    }
}
```

---

## 🧪 Known Limitations

- Manual OpenCV SDK path config  
- Portrait mode only  
- FPS may drop on weaker devices  
- Some filters not wired to UI yet

---

## 🌱 Future Enhancements

- [ ] UI sliders for filters (Sobel, Laplacian)  
- [ ] Orientation support  
- [ ] In-app recording  
- [ ] Live filter tuning  
- [ ] Profiling tools

---

## 📚 Dependencies

- OpenCV 4.6.0  
- Camera2 API  
- OpenGL ES 2.0  
- AndroidX, Kotlin

---

## 📄 License

Educational & Research use only. Created as part of an AI Research Internship.

---

## 🙋 Contact

📧 khushimalik3122@gmail.com  
🔗 [Portfolio](https://khushimalik3122.github.io/Khushi-portfolio-/)
