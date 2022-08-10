import os
import shutil
import platform
from pathlib import Path
import multiprocessing

cpu_count = multiprocessing.cpu_count()
project_dir = os.path.abspath(os.path.dirname(__file__))
build_dir = os.path.join(project_dir, 'build')

def setEnv(name, value):
    if platform.system() == 'Windows':
        os.system(f'set {name}={value}')
    else:
        os.system(f'export {name}={value}')
    os.environ[str(name)] = str(value)

def safeRemove(rm_path):
    try:
        if os.path.isfile(rm_path):
            os.remove(rm_path)
        else:
            shutil.rmtree(rm_path)
    except:
        pass

def collect_libs(target):
    libs_dir = os.path.dirname(project_dir)
    toolchain_dir = os.path.join(project_dir, 'app', 'toolchains', 'libs')
    safeRemove(toolchain_dir)
    os.makedirs(toolchain_dir)
    shutil.copytree(os.path.join(libs_dir, 'cmake'), os.path.join(toolchain_dir, 'cmake'))
    exclude = ['igeCreator', '.git', '.vscode']
    if target == 'windows':
        exclude.append('macos')
        exclude.append('ios')
    if target == 'macos':
        exclude.append('windows')
    for root, dirs, files in os.walk(libs_dir):
        dirs[:] = [d for d in dirs if d not in exclude]
        for _dir in dirs:
            src_path = os.path.join(root, _dir)
            dst_path = os.path.join(toolchain_dir, os.path.relpath(root, start = libs_dir), _dir)
            if((_dir == 'prebuilt' or _dir == 'single_include') and not os.path.exists(dst_path)):
                print(f'copying {src_path} -> {dst_path}')
                shutil.copytree(src_path, dst_path)
                for excl in exclude:
                    safeRemove(os.path.join(dst_path, excl))

def _generateCMakeProject(target, arch):
    cmake_cmd = f'cmake {project_dir} -DCMAKE_BUILD_TYPE=Release '
    if target == "windows":
        if arch == "x86":
            cmake_cmd += f' -A Win32'
        else:
            cmake_cmd += f' -A X64'
    elif target == "android":
        toolchain = Path(os.environ.get("ANDROID_NDK_ROOT")).absolute().as_posix() + '/build/cmake/android.toolchain.cmake'
        if arch == "armv7":
            cmake_cmd += f' -G Ninja -DCMAKE_TOOLCHAIN_FILE={toolchain} -DANDROID_ABI=armeabi-v7a -DANDROID_PLATFORM=android-21'
        else:
            cmake_cmd += f' -G Ninja -DCMAKE_TOOLCHAIN_FILE={toolchain} -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-21'
    elif target == "ios":
        toolchain = Path(self.source_folder).absolute().as_posix() + '/cmake/ios.toolchain.cmake'
        cmake_cmd += f' -G Xcode -DCMAKE_TOOLCHAIN_FILE={toolchain} -DIOS_DEPLOYMENT_TARGET=11.0 -DPLATFORM=OS64'
    elif target == "macos":
        cmake_cmd += f' -G Xcode -DOSX=1 -DCMAKE_OSX_ARCHITECTURES=x86_64'
    elif target == "emscripten":
        toolchain = Path(os.environ.get("EMSCRIPTEN_ROOT_PATH")).absolute().as_posix() + '/cmake/Modules/Platform/Emscripten.cmake'
        cmake_cmd += f' -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE={toolchain}'
    else:
        print(f'Configuration not supported: platform = {target}, arch = {arch}')
        exit(1)
    os.system(cmake_cmd)

def stripXcode(pbxproj):
    isProjectSection = False
    isWriting = True
    inputLines = None
    with open(pbxproj, 'r') as inputFile:
        inputLines = inputFile.readlines()
    with open(pbxproj, 'w') as outputFile:
        for line in inputLines:
            if '/* Begin PBXProject section */' in line:
                isProjectSection = True
            elif isProjectSection:
                if '/* End PBXProject section */' in line:
                    isProjectSection = False
                if 'buildSettings = {' in line or 'buildStyles = (' in line:
                    isWriting = False
            if not isWriting:
                if ');' in line or '};' in line:
                    isWriting = True
            else:
                outputFile.write(line)


def _buildCMakeProject(target, arch):
    if(target == 'macos'):
        stripXcode(os.path.join(build_dir, 'igeCreator.xcodeproj', 'project.pbxproj'))
    platform_flags = f"-- /p:CL_MPcount={cpu_count}" if target == "windows" else ""
    os.system(f'cmake --build . --config Release --target package --parallel {cpu_count} {platform_flags}')

def build(target, arch):
    global build_dir
    collect_libs(target)
    os.chdir(project_dir)
    build_dir = Path(os.path.join(project_dir, 'build', target, arch)).as_posix()
    safeRemove(build_dir)
    os.makedirs(build_dir)
    os.chdir(build_dir)
    _generateCMakeProject(target, arch)
    _buildCMakeProject(target, arch)
    os.chdir(project_dir)
    release_dir = Path(os.path.join(project_dir, 'release')).as_posix()
    safeRemove(release_dir)
    os.makedirs(release_dir)
    for fname in os.listdir(build_dir):
        if fname.endswith('.exe') or fname.endswith('.zip'):
            shutil.move(os.path.join(build_dir, fname), release_dir)

def main():
    if platform.system() == 'Windows':
        build('windows', 'x86_64')
    elif platform.system() == 'Darwin':
        build('macos', 'x86_64')

if __name__ == "__main__":
    main()    
