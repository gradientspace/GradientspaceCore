# GradientspaceCore

GradientspaceCore is an Open Source (MIT License) C++ library for geometry/mesh processing. This library is under active development and is conceptually an ongoing port of the C# [geometry3Sharp](https://github.com/gradientspace/geometry3Sharp) library to C++.

If you would like to quickly get up and running with this library, check out the [GradientspaceDemo](https://github.com/gradientspace/GradientspaceDemo) repo which will get you started with a command-line tool.

# Questions / Comments / etc

Email [rms@gradientspace.com](mailto:rms@gradientspace.com), find rms on [BlueSky](https://bsky.app/profile/rms80.bsky.social) or join the [Gradientspace Discord](https://discord.gg/2Dnjr5afSz)

# Warning

Note that GradientspaceCore and GradientspaceIO are under active development and no commitments have been made to API stability or backwards-compatibility at this time.

# Contributions

Bug reports are appreciated, and Feature Requests are nice to hear. PRs will be considered, however please check with us before developing any substantial PRs. This library is being developed with a particular philosophy around user-friendly APIs and similarity to the C# library, and that will be prioritized over features and capabilities.


# Feature Set

- templated Vector Math types, eg Vector2<T>, Vector3<T>, Quaternion, Matrix2, Matrix3
- ...



# Unreal Engine Compatibility

This library is structured so that it can (optionally) be directly used as a Module in an Unreal Engine 5 Plugin. The source is already in */Public* and */Private* folders, and a *GradientspaceCore.Build.cs* file is included which sets compiler defines that will result in (eg) the DLLMain being removed and a Module.h/cpp definition being included. If your plugin is named "MyPlugin", then you simply place the entire repository in the */MyPlugin/Source* folder, and add GradientspaceCore to the .uplugin modules list.

In addition, for many of the basic vector-math types like GS::Vector3d, GS::Index3i, etc, conversion operators will be conditionally included (based on `#define GS_ENABLE_UE_TYPE_COMPATIBILITY` added by the Build.cs). This allows for transparent conversion between GradientspaceCore and UE types (both UE Core and GeometryCore-specific types where they are different). Generally the conversions will be implicit unless there is narrowing (eg float-to-double, etc).

A sample UE project with a plugin setup will be published in future.



# Building

A sample vcxproj file for Visual Studio is provided in `/Build/vs2022/GradientspaceCore.vcxproj`. You should be able to add this directly to a VS2022 Solution.

A CMake script is also included, with presets. The preset **Windows x64 vs2022** / **windows-x64-vs2022** will generate a Visual Studio project in `Build/cmake-win64-vs2022`.

On Linux, use `cmake --preset=linux` to generate a makefile project in `Build/cmake-linux`. 

The CMake script is suitable to be included in higher-level scripts using add_directory(). See the [GradientspaceDemo](https://github.com/gradientspace/GradientspaceDemo) CMakeLists.txt for an example setup.

