# DependencyChaos
A visual dependency walker for MacOS Dylibs, Frameworks and Apps build with Qt5.8. This small project was created for the fast resolution of dependency problems. Manually with otool and install_name_tool search for dependencies is very complex.

You simply select the desired files, the program then shows whether all dependencies are present and which are missing. We consider the rpath, loader_path and executable_path. The toolbar offers the possibility to solve errors directly without having to enter commands via the commandline.

![Alt text](https://raw.githubusercontent.com/michaelgeschwill/DependencyChaos/master/Screenshot.png "DependencyChaos Samplescreen")

# Release
You can download prebuilded releases or build from code.
https://github.com/michaelgeschwill/DependencyChaos/releases

# How to build
Download or clone repository. Install Qt5.6 or higher. Set permissions to build.sh

`chmod +x build.sh`

Now build

`./build.sh`

The application will be store in /bin folder. That's all! ;)
