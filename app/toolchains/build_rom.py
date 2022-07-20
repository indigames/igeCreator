import igeCore as core
from igeCore import devtool
from igeCore import apputil

import os
from os import path

import shutil
import sys
import getopt

import build_pylibs
import convert_textures

verbose = False

def _errorRemoveReadonly(func, path, exc):
    excvalue = exc[1]
    if excvalue.errno == errno.EACCES:
        os.chmod(path, stat.S_IWUSR | stat.S_IWRITE | stat.S_IRWXU | stat.S_IRWXG | stat.S_IRWXO)
        func(path)
    else:
        raise ValueError("file {} cannot be deleted.".format(path))

def safeRemove(path):
    if not os.path.exists(path):
        return
    if os.path.isfile(path) or os.path.islink(path):
        os.remove(path)
    elif os.path.isdir(path):
        shutil.rmtree(path, ignore_errors=False, onerror=_errorRemoveReadonly)
    else:
        raise ValueError("file {} is not a file or dir.".format(path))

def copyFile(src, dst):
    apputil.makeDirectories(dst)
    shutil.copy(src, dst)

def build_rom(platform, projectDir, outputDir):
    tmp = path.join('.tmp', 'pc' if platform == core.TARGET_PLATFORM_PC else 'mobile')
    devtool.compilePrograms(projectDir, tmp)
    
    if platform == core.TARGET_PLATFORM_PC:
        if path.exists(path.join('.tmp', 'pcscripts')):
            shutil.move(path.join('.tmp', 'pcscripts'), path.join('.tmp', 'pc', 'scripts'))
    else:
        if path.exists(path.join('.tmp', 'mobilescripts')):
            shutil.move(path.join('.tmp', 'mobilescripts'), path.join('.tmp', 'mobile', 'scripts'))
    
    convert_textures.convertTextures(projectDir, outputDir, platform)
    devtool.copyFiles(projectDir, tmp, ['.plist', '.pem', '.json', '.pickle', '.zip', '.txt', '.db', '.ttf', '.otf', '.ogg', '.wav', '.mp3', '.mp4', '.dat', '.ini', '.sqlite', '.pyxd', '.scene', '.igeproj', '.prefab', '.pyxi', '.pyxf', '.pyxa', '.pybm', '.efk', '.config', '.anim', '.gltf'])
    processEffects(projectDir, tmp)
    dist = outputDir
    if os.path.isdir(dist):
        shutil.rmtree(dist)
    apputil.makeDirectories(dist)

    for root, dirs, files in os.walk(tmp):
        if root.find('\.') != -1: continue
        for fname in files:
            name, ext = os.path.splitext(fname)
            fpath = os.path.relpath(root, start = tmp)
            copyFile(os.path.join(root, fname), os.path.join(os.path.join(dist, fpath), fname))

def processEffects(projectDir, outputDir):
    effectOutDir = os.path.join(outputDir, 'effects')
    for root, dirs, files in os.walk(effectOutDir):
        for file in files:
            exts = ('.pyxi', '.pyxf', '.pyxa')
            if file.lower().endswith(exts):
                os.remove(os.path.join(root, file))
    
    effectInDir = os.path.join(projectDir, 'effects')
    devtool.copyFiles(effectInDir, effectOutDir, ['.dds', '.png', '.tga', '.fbx', '.efk', '.efkmat', '.efkmodel'])

def build(project_dir, output_dir):
    global verbose
    safeRemove(output_dir)
    build_pylibs.main(os.path.join(project_dir, 'config', 'app.yaml'), output_dir, verbose)
    build_rom(core.TARGET_PLATFORM_PC, project_dir, os.path.join(output_dir, 'pc'))
    build_rom(core.TARGET_PLATFORM_MOBILE, project_dir, os.path.join(output_dir, 'mobile'))
    shutil.move(os.path.join(output_dir, 'pc', 'config'), os.path.join(output_dir, 'config'))
    copyFile(os.path.join(project_dir, 'config', 'app.yaml'), os.path.join(output_dir, 'config'))
    safeRemove(os.path.join(output_dir, 'mobile', 'config'))
    safeRemove(os.path.join(output_dir, 'pc', 'config'))
    
    rom_file = os.path.join(output_dir, 'ROM.zip')
    if path.exists(rom_file):
        safeRemove(rom_file)    
    shutil.make_archive('ROM_tmp', 'zip', output_dir)
    shutil.move('ROM_tmp.zip', rom_file)

if __name__ == "__main__":
    project_dir = '../project'
    output_dir = '../dist'
    verbose = False

    try:
        opts, args = getopt.getopt(argv, "i:o:c:v:")
    except getopt.GetoptError:
        print('build_rom.py -i <input_dir> -o <output_dir> -v <verbose: True/False>')
        sys.exit(2)

    for opt, arg in opts:
        if opt in ("-i"):
            project_dir = arg
        elif opt in ("-o"):
            output_dir = arg
        elif opt in ("-v", "--verbose"):
            verbose = True

    build(project_dir, output_dir)
    