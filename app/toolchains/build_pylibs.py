import shutil
import os
import sys
import getopt
import platform
import errno
import stat
from os import path
from app_config import AppConfig

script_dir = path.abspath(path.dirname(__file__))
appConfig = AppConfig()
verbose = False

def _errorRemoveReadonly(func, path, exc):
    if exc[1].errno == errno.EACCES:
        os.chmod(path, stat.S_IWUSR | stat.S_IWRITE | stat.S_IRWXU | stat.S_IRWXG | stat.S_IRWXO)
        func(path)

def safeRemove(path):
    if not os.path.exists(path):
        return
    if os.path.isfile(path) or os.path.islink(path):
        os.remove(path)
    elif os.path.isdir(path):
        shutil.rmtree(path, ignore_errors=True, onerror=_errorRemoveReadonly)
    else:
        pass

def genPyLib(pylib_dir, output_dir):
    global verbose
    from igeCore import devtool
    src = pylib_dir
    tmp = path.join(output_dir, 'pyLib_tmp')

    if os.path.exists(tmp):
        safeRemove(tmp)
    shutil.copytree(src, tmp)

    if not appConfig.requires.get('pybox2d'):
        safeRemove(path.join(tmp, 'site-packages', 'Box2D'))

    if not appConfig.requires.get('opencv'):
        safeRemove(path.join(tmp, 'site-packages', 'cv2'))

    if not appConfig.requires.get('pyimgui'):
        safeRemove(path.join(tmp, 'site-packages', 'imgui'))

    if not appConfig.requires.get('igeEffekseer'):
        safeRemove(path.join(tmp, 'site-packages', 'igeEffekseer'))

    if not appConfig.requires.get('tensorflow'):
        safeRemove(path.join(tmp, 'site-packages', 'tflite_runtime'))

    if not appConfig.requires.get('pillow'):
        safeRemove(path.join(tmp, 'site-packages', 'PIL'))

    if not appConfig.requires.get('bullet'):
        safeRemove(path.join(tmp, 'site-packages', 'pybullet_data'))
        safeRemove(path.join(tmp, 'site-packages', 'pybullet_envs'))
        safeRemove(path.join(tmp, 'site-packages', 'pybullet_robots'))
        safeRemove(path.join(tmp, 'site-packages', 'pybullet_utils'))
        if not appConfig.requires.get('tensorflow'):
            safeRemove(path.join(tmp, 'site-packages', 'gym'))

    pylib_tmp = path.join(output_dir, '.tmp')
    if not os.path.exists(pylib_tmp):
        os.makedirs(pylib_tmp)

    if verbose:
        print('output_dir:', output_dir)
        print('tmp:', tmp)
        print('pylib_tmp:', pylib_tmp)

    devtool.compilePrograms(tmp, pylib_tmp)
    devtool.compressFolder(pylib_tmp)

    tmp_pylib = path.join(output_dir, '.tmp.pyxd')
    if os.path.exists(tmp_pylib):
        pylib = path.join(output_dir, 'PyLib.pyxd')
        safeRemove(pylib)
        os.rename(r'{}'.format(tmp_pylib), r'{}'.format(pylib))

    safeRemove(tmp)
    safeRemove(pylib_tmp)

def usage():
    print('gen-pylibs.py')
    print('    -h|--help : show help menu')
    print('    -c|--config : path to app.yaml')
    print('    -o|--outdir : output dir')
    print('    -v|--verbose : verbose')

def main(config_file, output_dir, debug):
    global appConfig
    global verbose
    verbose = debug

    if verbose:
        print('Config file: {}'.format(config_file))
        print('Output dir: {}'.format(output_dir))

    appConfig.readConfig(config_file)
    if verbose:
        print('Requires:\n')
        for key in appConfig.requires.keys():
            print(f"{key}: {appConfig.requires[key]}")
        print('\n')

    genPyLib(path.join(script_dir, 'PyLib'), output_dir)

if __name__ == "__main__":
    output_dir = path.join(os.getcwd(), 'release')
    config_file = path.join(os.getcwd(), 'app.yaml')

    try:
        argv = sys.argv[1:]
        opts, args = getopt.getopt(argv, "hc:o:v", ["help", "config=", "outdir=", "verbose"])
    except getopt.GetoptError:
        usage()
        sys.exit(2)

    for opt, arg in opts:
        if opt in ("-h", "--help"):
            usage()
            sys.exit()
        elif opt in ("-c", "--config"):
            if path.isabs(r'{}'.format(arg)):
                config_file = arg
            else:
                config_file = path.abspath(path.join(os.getcwd(), r'{}'.format(arg)))
        elif opt in ("-o", "--outdir"):
            if path.isabs(r'{}'.format(arg)):
                output_dir = arg
            else:
                output_dir = path.abspath(path.join(os.getcwd(), r'{}'.format(arg)))
        elif opt in ("-v", "--verbose"):
            verbose = True

    main(config_file, output_dir, verbose)
