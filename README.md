# OpenGL Project Base

This is a ready to go OpenGL project base with ImGUI interface.

## Building

``` bash
python -m venv env
source env/bin/activate
pip install scons pyyaml
scons
```

To build install scons, and pyyaml then run scons.

There is one build flag `GL_CORE_PROFILE`, setting that flag will enforce OpenGL
3.3 core, otherwise it will use a OpenGL 2 profile (not recommended).
