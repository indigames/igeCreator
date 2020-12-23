
import igeCore as core
from igeCore import devtool
from igeCore import apputil

import shutil
import os.path
import sys
import getopt

def copyFile(src, dst):	
	apputil.makeDirectories(dst)
	shutil.copy(src, dst)

def build(platform, projectDir, outputDir, compress):
	tmp = '.tmp/' + apputil.platformName(platform)
	devtool.compilePrograms(projectDir, tmp)
	devtool.convertAssets(projectDir, tmp, platform, 1.0)
	devtool.copyFiles(projectDir, tmp, ['.plist', '.pem', '.json', '.pickle', '.zip', '.txt', '.db', '.ttf', '.otf', '.ogg', '.wav', '.dat', '.ini', '.sqlite', '.pyxd', '.scene', '.prefab', '.efk', '.efkproj'])
	if compress:
		devtool.packFolders(tmp)
		
	dist = outputDir
	if os.path.isdir(dist):
		shutil.rmtree(dist)
	apputil.makeDirectories(dist)	

	for root, dirs, files in os.walk(tmp):
		if root.find('\.') != -1: continue
		for fname in files:			
			name, ext = os.path.splitext(fname)
			if compress:
				if ext == '.pyxd' or ext == '.py' or ext =='.ini':
					fpath = os.path.relpath(root, start = tmp)
					copyFile(os.path.join(root, fname), os.path.join(os.path.join(dist, fpath), fname))
			else:
				fpath = os.path.relpath(root, start = tmp)
				copyFile(os.path.join(root, fname), os.path.join(os.path.join(dist, fpath), fname))
	
def main(argv):
	platform = core.TARGET_PLATFORM_PC
	project_dir = '../project'
	output_dir = '../dist'
	compress = True
	
	try:
		opts, args = getopt.getopt(argv, "p:i:o:c:")
	except getopt.GetoptError:
		print('build.py -p <platform: 0 = PC, 1 = IOS, 2 = ANDROID> -i <input_dir> -o <output_dir> -c <compress_output: True/False>')
		sys.exit(2)

	for opt, arg in opts:
		if opt in ("-p"):
			platform = int(arg)
		elif opt in ("-i"):			
			project_dir = arg
		elif opt in ("-o"):
			output_dir = arg
		elif opt in("-c"):
			compress = not (arg == 'False' or arg == '0')

	build(platform, project_dir, output_dir, compress)

if __name__ == "__main__":
	main(sys.argv[1:])
