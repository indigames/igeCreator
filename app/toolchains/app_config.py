import yaml
import os
from os import path

class AppConfig:
    def __init__(self):
        self.name = "igeLauncher"
        self.label = "IGE Launcher"
        self.versionCode = '1'
        self.versionName = '0.0.1'
        self.bundleId = 'net.indigames.igelauncher'
        self.orientation = 'portrait'
        self.android_romDir = 'android'
        self.android_configDir = 'config/android/res'
        self.android_arm7 = False
        self.android_arm8 = True
        self.android_x86 = False
        self.android_x64 = False
        self.android_minSdk = '21'
        self.android_targetSdk = '29'
        self.permissions = {}
        self.features = {}
        self.pc_romDir = 'pc'
        self.pc_configDir = ''
        self.pc_x64 = True
        self.ios_romDir = 'android'
        self.ios_configDir = 'config/ios/Assets.xcassets'
        self.ios_arm64 = True
        self.ios_deployTarget = '11.0'
        self.ios_deviceFamily = '1,2'
        self.ios_devTeam = 'R2TWY42MN5'
        self.ios_codeSign = 'iPhone Distribution'
        self.ios_provisioningProfile = 'igeLauncher-Adhoc-IOS'
        self.ios_cameraUsage = 'This app accesses your camera to capture photos.'
        self.ios_libraryUsage = 'This app accesses your photo library to search and display photos.'
        self.macos_romDir = 'pc'
        self.macos_configDir = 'config/macos/Assets.xcassets'
        self.ems_romDir = 'pc'
        self.ems_configDir = ''
        self.macos_x64 = True
        self.requires = {}

    def readConfig(self, config_file):
        with open(config_file, 'r') as f:
            config_path = os.path.dirname(os.path.normpath(config_file))
            data = yaml.load(f, Loader=yaml.FullLoader)
            if data is None: return
                
            # App information
            if data.get('App'):
                self.name = data['App'].get('Name', self.name)
                self.label = data['App'].get('Label', self.label)
                self.versionCode = '{}'.format(data['App'].get('VersionCode', self.versionCode))
                self.versionName = '{}'.format(data['App'].get('VersionName', self.versionName))
                self.bundleId = data['App'].get('BundleId', self.bundleId)
                self.orientation = data['App'].get('Orientation', self.orientation)

            # Android
            if data.get('Platforms'):
                if data['Platforms'].get('android'):
                    self.android_romDir = data['Platforms']['android'].get('RomDir', self.android_romDir)
                    self.android_configDir = data['Platforms']['android'].get('ConfigDir', self.android_configDir)
                    self.android_arm7 = data['Platforms']['android'].get('armeabi-v7a', self.android_arm7)
                    self.android_arm8 = data['Platforms']['android'].get('arm64-v8a', self.android_arm8)
                    self.android_x86 = data['Platforms']['android'].get('x86', self.android_x86)
                    self.android_x64 = data['Platforms']['android'].get('x86_64', self.android_x64)
                    self.android_minSdk = '{}'.format(data['Platforms']['android'].get('MinSdkVersion', self.android_minSdk))
                    self.android_targetSdk = '{}'.format(data['Platforms']['android'].get('TargetSdkVersion', self.android_targetSdk))
                    self.permissions = data['Platforms']['android'].get('Permissions', self.permissions)
                    self.features = data['Platforms']['android'].get('Features', self.features)

                # PC
                if data['Platforms'].get('windows'):
                    self.pc_romDir = data['Platforms']['windows'].get('RomDir', self.pc_romDir)
                    self.pc_configDir = data['Platforms']['windows'].get('ConfigDir', self.pc_configDir)
                    self.pc_x64 = data['Platforms']['windows'].get('x64', self.pc_x64)

                # iOS
                if data['Platforms'].get('ios'):
                    self.ios_romDir = data['Platforms']['ios'].get('RomDir', self.ios_romDir)
                    self.ios_configDir = data['Platforms']['ios'].get('ConfigDir', self.ios_configDir)
                    self.ios_arm64 = data['Platforms']['ios'].get('arm64', self.ios_arm64)
                    self.ios_deployTarget = '{}'.format(data['Platforms']['ios'].get('DeploymentTarget', self.ios_deployTarget))
                    self.ios_deviceFamily = data['Platforms']['ios'].get('DeviveFamily', self.ios_deviceFamily)
                    self.ios_devTeam = data['Platforms']['ios'].get('DevelopmentTeamID', self.ios_devTeam)
                    self.ios_codeSign = data['Platforms']['ios'].get('CodeSignIdentity', self.ios_codeSign)
                    self.ios_provisioningProfile = data['Platforms']['ios'].get('ProvisioningProfile', self.ios_provisioningProfile)
                    self.ios_cameraUsage = data['Platforms']['ios'].get('cameraUsage', self.ios_cameraUsage)
                    self.ios_libraryUsage = data['Platforms']['ios'].get('libraryUsage', self.ios_libraryUsage)

                # MacOS
                if data['Platforms'].get('macos'):
                    self.macos_romDir = data['Platforms']['macos'].get('RomDir', self.macos_x64)
                    self.macos_configDir = data['Platforms']['macos'].get('ConfigDir', self.macos_configDir)
                    self.macos_x64 = data['Platforms']['macos'].get('x64', self.macos_x64)

            # Requires modules
            if data.get('Requires'):
                for require in data.get('Requires'):
                    self.requires[require] = True
