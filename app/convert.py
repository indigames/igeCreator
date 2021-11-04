import os
from os import path
import igeCore
from igeCore import devtool

import sys
import getopt

def convertAssets(sourceDir, destDir, platform, unit=1.0, rootDir=None):
    """
    Convert model and image data to ige format
    model(.dae) -> .pyxf
    image(.png .dds .tga .psd ) -> .pyxi

    If model data with the same name as the folder name is found,
    it is regarded as base model data, and other model data in the
    same folder are regarded as motion files, and all are packed
    into one file
    If there is no model data with the same name as the folder name,
    Each model data as base model data and convert separately

    :param sourceDir: source root folder
    :param destDir: destination root folder
    :param platform: target platform (core.TARGET_PLATFORM_XX)
    :param unit: scale unit size of 3d model
    :return: None
    """
    if rootDir is None: rootDir = destDir

    imageList = devtool.findImageFiles(sourceDir)
    allTextures = dict()
    for img in imageList:
        if img.find('\\config\\') != -1 or img.find('/config/') != -1:
            continue
        outFile = devtool.replaceExt(img, '.pyxi');
        if not path.exists(outFile) and img not in allTextures:
            allTextures[img] = {'path': devtool.removeRoot(img, sourceDir), 'normal': False, 'wrap': False}
    if len(allTextures) > 0:
        devtool.convertImages(allTextures, sourceDir, destDir, platform)

def main(argv):
    project_dir = '.'
    output_dir = '.'

    try:
        opts, args = getopt.getopt(argv, "i:o")
    except getopt.GetoptError:
        print('convert.py -i <project_dir> -o <output_dir>')
        sys.exit(2)

    for opt, arg in opts:
        if opt in ("-i"):
            project_dir = arg
        elif opt in ("-o"):
            output_dir = arg
        
    convertAssets(project_dir, output_dir, igeCore.TARGET_PLATFORM_MOBILE)

if __name__ == "__main__":
    main(sys.argv[1:])
