import os
from os import path
import igeCore
from igeCore import devtool

import sys
import getopt

def convertTextures(sourceDir, destDir, platform):
    """
    Convert images (.png .dds .tga .psd ) to ige format (.pyxi)
    :param sourceDir: source root folder
    :param destDir: destination root folder
    :param platform: target platform (core.TARGET_PLATFORM_XX)
    :return: None
    """

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
        print('convert_textures.py -i <project_dir> -o <output_dir>')
        sys.exit(2)

    for opt, arg in opts:
        if opt in ("-i"):
            project_dir = arg
        elif opt in ("-o"):
            output_dir = arg
        
    convertTextures(project_dir, output_dir, igeCore.TARGET_PLATFORM_MOBILE)

if __name__ == "__main__":
    main(sys.argv[1:])
