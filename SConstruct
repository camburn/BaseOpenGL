import os
import sys
import glob
import platform
import re
import yaml

env = Environment()

mode = ARGUMENTS.get('target', 'debug')

path_matcher = re.compile(r'.*\$\{([^}^{]+)\}.*')
def path_constructor(loader, node):
    return os.path.expandvars(node.value)

class EnvVarLoader(yaml.SafeLoader):
    pass

EnvVarLoader.add_implicit_resolver('!path', path_matcher, None)
EnvVarLoader.add_constructor('!path', path_constructor)

if platform.system() == 'Windows':
    env.Append(
        TARGET_ARCH='x86_64'
    )
    with open('config/windows_x64.yaml', 'r') as f:
        try:
            data = yaml.load(f.read(), Loader=EnvVarLoader)
        except KeyError as err:
            print("Required build environment variable missing", err)
            sys.exit(1)

elif platform.system() == 'Linux':
    env['CXX'] = "clang++"
    env.Append(
        TARGET_ARCH='x86_64'
    )
    with open('config/linux.yaml', 'r') as f:
        try:
            data = yaml.load(f.read(), Loader=EnvVarLoader)
        except KeyError as err:
            print("Required build environment variable missing", err)
            sys.exit(1)
else:
    raise Exception("OS Not Supported")

env.Append(
    CXXFLAGS=data['compile_flags'],
    LINKFLAGS=data['link_flags'],
    ENV={'TERM': os.environ['TERM']}
)
if mode == 'debug':
    print('Building Debug Version')
    env.Append(
        CXXFLAGS=data['debug_compile_flags'],
        LINKFLAGS=data['debug_link_flags']
    )

# Need to work out when this is appropriate
if 'GL_CORE_PROFILE' in os.environ:
    env.Append(CPPDEFINES=['GL_CORE_PROFILE'])
env.VariantDir('build', 'src', duplicate=0)
env.Program(
    'bin/MyProject',
    Glob('extern/imgui/*.cpp') +
    Glob('extern/nanovg/*.c') +
    Glob('extern/gl3w/src/*.c') +
    Glob('build/*.cpp') +
    Glob('build/**/*.cpp'),
    CPPPATH=data['include_dirs'],
    LIBS=data['libs'],
    LIBPATH=data['lib_paths']
)
