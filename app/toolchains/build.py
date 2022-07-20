import shutil
from distutils import dir_util
import os
from os import path
import sys
import getopt
import subprocess
import fnmatch
import codecs
import json
import zipfile
import errno

import build_rom

from app_config import AppConfig

script_dir = path.abspath(path.dirname(__file__))
output_dir = path.join(os.getcwd(), 'project')
template_dir = path.abspath(path.join(script_dir, 'project_template'))
rom_dir = 'rom'
appConfig = AppConfig()

gen_windows = False
gen_android = False
gen_ios = False
gen_macos = False
gen_emscripten = False

verbose = False
debug = False

def findReplace(directory, find, replace, filePatterns):
    for path, dirs, files in os.walk(os.path.abspath(directory)):
        for filePattern in filePatterns:
            for filename in fnmatch.filter(files, filePattern):
                filepath = os.path.join(path, filename)
                with codecs.open(filepath, encoding='utf-8') as f:
                    s = f.read()
                s = s.replace(find, replace)
                with codecs.open(filepath, encoding='utf-8', mode='w') as f:
                    f.write(s)

def setEnv(name, value):
    if os.name == 'nt':
        return f'set {name}={value}\n'
    return f'export {name}={value}\n'

def setAppEnvs(platform):
    dst = path.join(output_dir, platform)
    if os.name == 'nt':
        filepath = path.join(dst, 'setEnv.bat')
    else:
        filepath = path.join(dst, 'setEnv.sh')
    with codecs.open(filepath, encoding='utf-8', mode='w') as f:
        if os.name == 'nt':
            f.write('@echo OFF\n')
        else:
            f.write('#!/bin/bash \n')
        f.write(setEnv("APP_NAME", appConfig.name))
        f.write(setEnv("APP_LABEL", f'\"{appConfig.label}\"'))
        f.write(setEnv("PACKAGE_NAME", appConfig.bundleId))
        f.write(setEnv("VERSION_CODE", appConfig.versionCode))
        f.write(setEnv("VERSION_NAME", appConfig.versionName))

def toggleCMakePackage(platform):
    dst = path.join(output_dir, platform)
    cmake_deps = ""
    for key in appConfig.requires.keys():
        val = appConfig.requires.get(key)
        cmake_deps = cmake_deps + f"set(USE_{key.upper()} ON)\n"
    findReplace(dst, '#CMAKE_DEPENDENCIES#', cmake_deps, ['CMakeLists.txt'])

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

def cloneFromTemplate(platform):
    src = path.join(template_dir, platform)
    dst = path.join(output_dir, platform)
    if verbose:
        print('Cloning:\n\tfrom {}\n\tto {}'.format(src, dst))
    safeRemove(dst)    
    shutil.copytree(src, dst)
    
    safeRemove(path.join(output_dir, 'src'))
    shutil.copytree(path.join(template_dir, 'src'), path.join(output_dir, 'src'))

    igeSdkConfigPath = os.path.abspath(os.path.realpath(os.path.join(rom_dir, "config", "igeSdk.config")))
    pylib = os.path.join(rom_dir, "PyLib.pyxd")
    data_folder = path.join(dst, 'app')
    platform_rom_dir = path.join(rom_dir, appConfig.pc_romDir)
    platform_config_dir = path.join(rom_dir, appConfig.pc_configDir)

    if platform == 'android':
        data_folder = path.join(dst,'app/src/main/assets/data')
        platform_rom_dir = path.join(rom_dir, appConfig.android_romDir)
        platform_config_dir = path.join(rom_dir, appConfig.android_configDir)
    elif platform == 'ios':
        data_folder = path.join(dst,'.bundle')
        platform_rom_dir = path.join(rom_dir, appConfig.ios_romDir)
        platform_config_dir = path.join(rom_dir, appConfig.ios_configDir)
    elif platform == 'macos':
        data_folder = path.join(dst,'.bundle')
        platform_rom_dir = path.join(rom_dir, appConfig.macos_romDir)
        platform_config_dir = path.join(rom_dir, appConfig.macos_configDir)
    elif platform == 'emscripten':
        platform_rom_dir = path.join(rom_dir, appConfig.ems_romDir)
        platform_config_dir = path.join(rom_dir, appConfig.ems_configDir)

    if not path.exists(data_folder):
        os.makedirs(data_folder)

    print(f"platform_rom_dir = {platform_rom_dir}")
    print(f"platform_config_dir = {platform_config_dir}")
    if path.exists(platform_rom_dir):
        dir_util.copy_tree(platform_rom_dir, data_folder)
        print(f"platform_rom_dir => copy_tree {platform_rom_dir}")
        
    if path.exists(platform_config_dir):
        dir_util.copy_tree(platform_config_dir, data_folder)
        print(f"platform_config_dir => copy_tree {platform_config_dir}")
        
    if path.exists(igeSdkConfigPath):
        shutil.copy(igeSdkConfigPath, data_folder)

    if path.exists(pylib):
        shutil.copy(pylib, data_folder)

    if verbose:
        print('Cloning DONE!')

def processProject(platform):
    dst = path.join(output_dir, platform)
    if verbose:
        print('Processing {}...'.format(dst))

    # processing source code
    if path.exists(path.join(dst, 'app/src/main/java/net/indigames/igeSample')):
        bundleName = appConfig.bundleId.split(".")[-1].lower()
        os.rename(path.join(dst, 'app/src/main/java/net/indigames/igeSample'), path.join(dst, 'app/src/main/java/net/indigames/' + bundleName))

    if not appConfig.requires.get('opencv'):
        safeRemove(path.join(dst, 'app/src/main/java/org/opencv'))

    if not appConfig.requires.get('igeSocial'):
        safeRemove(path.join(dst, 'app/src/main/java/net/indigames/lib/social'))
        safeRemove(path.join(dst, 'app/src/main/java/net/indigames/lib/SocialHelper.java'))

    if not appConfig.requires.get('igeNotify'):
        safeRemove(path.join(dst, 'app/src/main/java/net/indigames/lib/notification'))

    if not appConfig.requires.get('igeWebview'):
        safeRemove(path.join(dst, 'app/src/main/java/net/indigames/lib/webview'))
        safeRemove(path.join(dst, 'app/src/main/java/net/indigames/lib/WebViewHelper.java'))

    if not appConfig.requires.get('igeSdk'):
        safeRemove(path.join(dst, 'app/src/main/java/net/indigames/lib/iap'))
        safeRemove(path.join(dst, 'app/src/main/java/net/indigames/lib/AppLovinMax.java'))

    # processing android specific
    if platform == 'android':
        findReplace(dst, '#MIN_SDK_VERSION#', appConfig.android_minSdk, ['*.gradle'])
        findReplace(dst, '#TARGET_SDK_VERSION#', appConfig.android_targetSdk, ['*.gradle'])

        # processing gradle
        abiFilters = None
        if appConfig.android_arm7:
            abiFilters = "'armeabi-v7a'"
        if appConfig.android_arm8:
            abiFilters = "'arm64-v8a'" if abiFilters == None else abiFilters + ", 'arm64-v8a'"
        if appConfig.android_x86:
            abiFilters = "'x86'" if abiFilters == None else abiFilters + ", 'x86'"
        if appConfig.android_x64:
            abiFilters = "'x86_64'" if abiFilters == None else abiFilters + ", 'x86_64'"
        findReplace(dst, '#ANDROID_ABI_FILTERS#', abiFilters, ['*.gradle'])

        impl = '''// igeSocial
    implementation "com.google.android.gms:play-services-games:${project.GMS_PLAY_SERVICES_GAMES_VERSION}"
    implementation "com.google.android.gms:play-services-auth:$project.GMS_PLAY_SERVICES_AUTH_VERSION"
    implementation "com.facebook.android:facebook-share:$project.FACEBOOK_SDK_VERSION"
    implementation "com.facebook.android:facebook-login:$project.FACEBOOK_SDK_VERSION"'''
        findReplace(dst, '#IGE_SOCIAL_GRADLE_IMPL#', impl if appConfig.requires.get('igeSocial') else '', ['build.gradle'])

        impl = '''// igeSocial plugin
apply plugin: "com.google.gms.google-services"'''
        findReplace(dst, '#IGE_SOCIAL_GRADLE_PLUGIN#', impl if appConfig.requires.get('igeSocial') else '', ['build.gradle'])

        impl = '''// igeNotify
    implementation "androidx.work:work-runtime:$project.WORK_RUNTIME_VERSION"'''
        findReplace(dst, '#IGE_NOTIFY_GRADLE_IMPL#', impl if appConfig.requires.get('igeNotify') else '', ['build.gradle'])

        impl = ''' // igeWebview
    implementation "com.just.agentweb:agentweb:$project.AGENT_WEB_VERSION"'''
        findReplace(dst, '#IGE_WEBVIEW_GRADLE_IMPL#', impl if appConfig.requires.get('igeWebview') else '', ['build.gradle'])

        impl = '''// igeCamera
    implementation "androidx.camera:camera-camera2:$project.CAMERAX_VERSION"
    implementation "androidx.camera:camera-core:$project.CAMERAX_VERSION"'''
        findReplace(dst, '#IGE_CAMERA_GRADLE_IMPL#', impl if appConfig.requires.get('igeCamera') else '', ['build.gradle'])

        impl = '''// igeFirebase plugin
apply from: "$gradle.firebase_cpp_sdk_dir/Android/firebase_dependencies.gradle"
firebaseCpp.dependencies { analytics admob auth messaging remoteConfig firestore }'''
        findReplace(dst, '#IGE_FIREBASE_GRADLE_PLUGIN#', impl if appConfig.requires.get('igeFirebase') else '', ['build.gradle'])

        # processing Android Manifest
        findReplace(dst, '#SCREEN_ORIENTATION#', 'portrait' if appConfig.orientation.lower() == 'portrait' else 'landscape', ['Androidmanifest.xml'])

        androidPermissions = ''
        usePermission = '<uses-permission android:name="{}"/>\n'
        for permission in appConfig.permissions:
            androidPermissions = androidPermissions + ('    ' if androidPermissions != '' else '') + usePermission.format(permission)
        findReplace(dst, '#ANDROID_PERMISSIONS#', androidPermissions, ['Androidmanifest.xml'])

        fireBasePermission = '''<!-- Firebase permissions -->
            <uses-permission android:name="com.google.android.c2dm.permission.RECEIVE"/>
            <permission android:name="#PACKAGE_NAME#.permission.C2D_MESSAGE" android:protectionLevel="signature"/>
            <uses-permission android:name="#PACKAGE_NAME#.permission.C2D_MESSAGE"/>'''
        findReplace(dst, '#FIREBASE_PERMISSIONS#', fireBasePermission if appConfig.requires.get('igeFirebase') else '', ['Androidmanifest.xml'])

        androidFeatures = ''
        useFeature = '<uses-feature android:name="{}" android:required="{}"/>\n'
        for k,v in appConfig.features.items():
            androidFeatures = androidFeatures + ('    ' if androidFeatures != '' else '') + useFeature.format(k, 'true' if v else 'false')
        findReplace(dst, '#ANDROID_FEATURES#', androidFeatures , ['Androidmanifest.xml'])

        meta = '''<!-- Local PN -->
        <receiver android:name="net.indigames.lib.notification.IGENotificationPublisher"></receiver>'''
        findReplace(dst, '#IGE_NOTIFY_METADATA#', meta if appConfig.requires.get('igeNotify') else '', ['Androidmanifest.xml'])

        meta = '''<!-- Webview Activity -->
        <activity android:name="net.indigames.lib.webview.IGEWebActivity" android:exported="true" android:launchMode="singleTop" android:theme="@style/AppFullScreenTheme"></activity>'''
        findReplace(dst, '#IGE_WEBVIEW_METADATA#', meta if appConfig.requires.get('igeWebview') else '', ['Androidmanifest.xml'])

        meta = '''<!-- Camera Preview Activity -->
        <activity android:name="net.indigames.lib.mlkit.LivePreviewActivity" android:exported="true" android:launchMode="singleTop" android:theme="@style/AppFullScreenTheme"></activity>'''
        findReplace(dst, '#IGE_CAMERA_METADATA#', meta if appConfig.requires.get('igeCamera') else '', ['Androidmanifest.xml'])

        meta = '''<!-- Facebook -->
        <meta-data android:name="com.facebook.sdk.ApplicationName" android:value="@string/app_name"></meta-data>
        <meta-data android:name="com.facebook.sdk.ApplicationId" android:value="@string/facebook_app_id"></meta-data>
        <meta-data android:name="com.facebook.sdk.ClientToken" android:value="@string/fb_client_token"/>

        <!-- Goolge AdMob -->
        <meta-data android:name="com.google.android.gms.ads.APPLICATION_ID" android:value="@string/gms_ads_APP_ID"></meta-data>
        <meta-data android:name="com.google.android.gms.ads.AD_MANAGER_APP" android:value="true"/>

        <!-- AppsFlyer -->
        <receiver android:name="com.appsflyer.SingleInstallBroadcastReceiver"
            android:exported="true">
            <intent-filter>
                <action android:name="com.android.vending.INSTALL_REFERRER" />
            </intent-filter>
        </receiver>'''
        findReplace(dst, '#IGE_SDK_METADATA#', meta if appConfig.requires.get('igeSdk') else '', ['Androidmanifest.xml'])

        meta = '''<!-- Fire-base meta-data -->
        <service android:name="com.google.firebase.messaging.cpp.RegistrationIntentService"
            android:exported="false"/>
        <service android:name="com.google.firebase.messaging.MessageForwardingService"
            android:exported="false"/>
        <service android:name="com.google.firebase.messaging.cpp.ListenerService"
            android:exported="false">
            <intent-filter>
                <action android:name="com.google.firebase.MESSAGING_EVENT"></action>
            </intent-filter>
        </service>'''
        findReplace(dst, '#IGE_FIREBASE_METADATA#', meta if appConfig.requires.get('igeFirebase') else '', ['Androidmanifest.xml'])

        meta = '''<!-- Social meta-data -->
        <meta-data android:name="com.google.android.gms.games.APP_ID"
            android:value="@string/gms_games_APP_ID" />
        <meta-data android:name="com.google.android.gms.version"
            android:value="@integer/google_play_services_version"/>
        <provider android:name="com.facebook.FacebookContentProvider"
            android:authorities="@string/facebook_app_authorities"
            android:exported="true" />'''
        findReplace(dst, '#IGE_SOCIAL_METADATA#', meta if appConfig.requires.get('igeSocial') else '', ['Androidmanifest.xml'])

    if platform == 'ios':
        findReplace(dst, '#DEPLOYMENT_TARGET#', appConfig.ios_deployTarget, ['CMakeLists.txt'])
        findReplace(dst, '#DEVICE_FAMILY#', appConfig.ios_deviceFamily, ['CMakeLists.txt'])
        findReplace(dst, '#DEVELOPMENT_TEAM_ID#', appConfig.ios_devTeam, ['CMakeLists.txt'])
        findReplace(dst, '#CODE_SIGN_IDENTITY#', appConfig.ios_codeSign, ['CMakeLists.txt'])
        findReplace(dst, '#PROVISIONING_PROFILE_NAME#', appConfig.ios_provisioningProfile, ['CMakeLists.txt'])
        findReplace(dst, '#CAMERA_USAGE_DESC#', appConfig.ios_cameraUsage, ['Info.plist.in'])
        findReplace(dst, '#PHOTO_LIBRARY_USAGE_DESC#', appConfig.ios_libraryUsage, ['Info.plist.in'])
        findReplace(dst, '#SCREEN_ORIENTATION#', 'UIInterfaceOrientationPortrait' if appConfig.orientation.lower() == 'portrait' else 'UIInterfaceOrientationLandscapeLeft', ['Info.plist.in'])

        # igeSocials
        pod = '''# igeSocials
    pod 'FBSDKLoginKit', '6.5.1'
    pod 'FBSDKShareKit', '6.5.1'
    '''
        findReplace(dst, '#IGE_SOCIAL_PODS#', pod if appConfig.requires.get('igeSocials') else '', ['Podfile'])

        # igeFirebase
        pod = '''# igeFirebase
    pod 'Firebase/Analytics', '6.32.2'
    pod 'Firebase/AdMob', '6.32.2'
    pod 'Firebase/Auth', '6.32.2'
    pod 'Firebase/Functions', '6.32.2'
    pod 'Firebase/Messaging', '6.32.2'
    pod 'Firebase/Storage', '6.32.2'
    pod 'Firebase/Firestore', '6.32.2'
    pod 'Firebase/DynamicLinks', '6.32.2'
    pod 'Firebase/Database', '6.32.2'
    pod 'Firebase/RemoteConfig', '6.32.2'
    '''
        findReplace(dst, '#IGE_FIREBASE_PODS#', pod if appConfig.requires.get('igeFirebase') else '', ['Podfile'])

    # processing App Info
    findReplace(dst, '#TARGET_NAME#', appConfig.name, ['CMakeLists.txt', '*.cmake', '*.java', '*.kt'])
    findReplace(dst, '#APP_NAME#', appConfig.label, ['CMakeLists.txt', '*.cmake', '*.xml'])
    findReplace(dst, '#VERSION_CODE#', appConfig.versionCode, ['CMakeLists.txt', '*.gradle'])
    findReplace(dst, '#VERSION_NAME#', appConfig.versionName, ['CMakeLists.txt', '*.gradle'])
    findReplace(dst, '#PACKAGE_NAME#', appConfig.bundleId, ['CMakeLists.txt', '*.gradle', '*.xml', '*.java', '*.kt'])

    # processing igeSdk
    igeSdkConfigPath = os.path.abspath(os.path.realpath(os.path.join(rom_dir, "config", "igeSdk.config")))
    androidConfigLoaded = False
    iosConfigLoaded = False
    try:
        with open(igeSdkConfigPath, 'r') as f:
            igeSdkConfig = json.load(f)
            if "android" in igeSdkConfig: # Android config
                sdkConfig = igeSdkConfig["android"]
                findReplace(dst, '#FACEBOOK_APP_ID#', sdkConfig["FacebookAppId"] if "FacebookAppId" in sdkConfig and len(sdkConfig["FacebookAppId"]) > 0 else "123456789012345", ['strings.xml'])
                findReplace(dst, '#FACEBOOK_CLIENT_TOKEN#', sdkConfig["FacebookClientToken"] if "FacebookClientToken" in sdkConfig and len(sdkConfig["FacebookClientToken"]) > 0 else "1234567890123456789012", ['strings.xml'])
                findReplace(dst, '#GAD_APP_ID#', sdkConfig["AdMobAppId"] if "AdMobAppId" in sdkConfig and len(sdkConfig["AdMobAppId"]) > 0 else "ca-app-pub-3940256099942544~3347511713", ['strings.xml'])
                findReplace(dst, '#GGAME_APP_ID#', sdkConfig["GoogleGameAppId"] if "GoogleGameAppId" in sdkConfig and len(sdkConfig["GoogleGameAppId"]) > 0 else "123456789012", ['strings.xml'])
                findReplace(dst, '#IAP_BASE64_ENCODED_PUBLIC_KEY#', sdkConfig["IAPPublicKey"] if "IAPPublicKey" in sdkConfig and len(sdkConfig["IAPPublicKey"]) > 0 else "", ['*.java'])

                impl = '''// igeSdk
    implementation "com.android.installreferrer:installreferrer:$project.INSTALL_REFERRER_VERSION"
    implementation "com.facebook.android:facebook-android-sdk:$project.FACEBOOK_SDK_VERSION"
    implementation "com.appsflyer:af-android-sdk:$project.APPSFLYER_SDK_VERSION"
    implementation "com.gameanalytics.sdk:gameanalytics-android:$project.GAMEANALYTICS_VERSION"'''

                if "Mediation" in sdkConfig and len(sdkConfig["Mediation"]) > 0 and sdkConfig["Mediation"].upper() == "MAX":
                    impl = impl + '''
    implementation 'com.android.billingclient:billing:4.1.0'
    implementation 'com.google.code.gson:gson:2.9.0'
    implementation 'com.applovin:applovin-sdk:+'
    implementation 'com.applovin.mediation:adcolony-adapter:+'
    implementation 'com.applovin.mediation:chartboost-adapter:+'
    implementation 'com.google.android.gms:play-services-base:18.0.1'
    implementation 'com.applovin.mediation:fyber-adapter:+'
    implementation 'com.applovin.mediation:google-ad-manager-adapter:+'
    implementation 'com.applovin.mediation:google-adapter:+'
    implementation 'com.applovin.mediation:hyprmx-adapter:+'
    implementation 'com.applovin.mediation:inmobi-adapter:+'
    implementation 'com.squareup.picasso:picasso:2.71828'
    implementation 'androidx.recyclerview:recyclerview:1.1.0'
    implementation 'com.applovin.mediation:ironsource-adapter:+'
    implementation 'com.applovin.mediation:line-adapter:+'
    implementation 'com.applovin.mediation:facebook-adapter:+'
    implementation 'com.applovin.mediation:mintegral-adapter:+'
    implementation 'com.applovin.mediation:mytarget-adapter:+'
    implementation 'com.applovin.mediation:ogury-presage-adapter:+'
    implementation 'com.applovin.mediation:bytedance-adapter:+'
    implementation 'com.applovin.mediation:snap-adapter:+'
    implementation 'androidx.constraintlayout:constraintlayout:1.1.3'
    implementation 'com.applovin.mediation:tapjoy-adapter:+'
    implementation 'com.applovin.mediation:unityads-adapter:+'
    implementation 'com.applovin.mediation:vungle-adapter:+'
    '''
                else:
                    safeRemove(path.join(dst, 'app/src/main/java/net/indigames/lib/AppLovinMax.java'))
                findReplace(dst, '#IGE_SDK_GRADLE_IMPL#', impl if appConfig.requires.get('igeSdk') else '', ['build.gradle'])

                repos = "maven { url 'https://maven.gameanalytics.com/release' }"
                if "Mediation" in sdkConfig and len(sdkConfig["Mediation"]) > 0 and sdkConfig["Mediation"].upper() == "MAX":
                    repos = repos + '''
            maven { url "https://android-sdk.is.com" }
            maven { url "https://dl-maven-android.mintegral.com/repository/mbridge_android_sdk_oversea" }
            maven { url "https://maven.ogury.co" }
            maven { url "https://artifact.bytedance.com/repository/pangle" }
            maven { url "https://sdk.tapjoy.com" }
            '''
                findReplace(dst, '#IGE_SDK_GRADLE_REPOS#', repos if appConfig.requires.get('igeSdk') else '', ['build.gradle'])
                androidConfigLoaded = True

            if "ios" in igeSdkConfig: # IOS config
                sdkConfig = igeSdkConfig["ios"]
                findReplace(dst, '#FACEBOOK_APP_ID#', sdkConfig["FacebookAppId"] if "FacebookAppId" in sdkConfig and len(sdkConfig["FacebookAppId"]) > 0 else "123456789012345", ['Info.plist.in'])
                findReplace(dst, '#FACEBOOK_CLIENT_TOKEN#', sdkConfig["FacebookClientToken"] if "FacebookClientToken" in sdkConfig and len(sdkConfig["FacebookClientToken"]) > 0 else "1234567890123456789012", ['Info.plist.in'])
                findReplace(dst, '#GAD_APP_ID#', sdkConfig["AdMobAppId"] if "AdMobAppId" in sdkConfig and len(sdkConfig["AdMobAppId"]) > 0 else "ca-app-pub-3940256099942544~3347511713", ['Info.plist.in'])
                findReplace(dst, '#GGAME_APP_ID#', sdkConfig["GoogleGameAppId"] if "GoogleGameAppId" in sdkConfig and len(sdkConfig["GoogleGameAppId"]) > 0 else "123456789012", ['Info.plist.in'])
                findReplace(dst, '#APPSTORE_APP_ID#', sdkConfig["AppsFlyerAppId"] if "AppsFlyerAppId" in sdkConfig and len(sdkConfig["AppsFlyerAppId"]) > 0 else "123456789012", ['Info.plist.in'])
                # igeSdk
                pod = '''# igeSdk
    pod 'FBSDKCoreKit'
    pod 'AppLovinSDK'
    pod 'AppsFlyerFramework'
    pod 'GA-SDK-IOS'
    '''
                if "Mediation" in sdkConfig and len(sdkConfig["Mediation"]) > 0 and sdkConfig["Mediation"].upper() == "MAX":
                    pod = pod + '''
    pod 'AppLovinMediationAdColonyAdapter'
    pod 'AppLovinMediationChartboostAdapter'
    pod 'AppLovinMediationFyberAdapter'
    pod 'AppLovinMediationGoogleAdManagerAdapter'
    pod 'AppLovinMediationGoogleAdapter'
    pod 'AppLovinMediationHyprMXAdapter'
    pod 'AppLovinMediationInMobiAdapter'
    pod 'AppLovinMediationIronSourceAdapter'
    pod 'AppLovinMediationLineAdapter'
    pod 'AppLovinMediationFacebookAdapter'
    pod 'AppLovinMediationMintegralAdapter'
    pod 'AppLovinMediationMyTargetAdapter'
    pod 'AppLovinMediationOguryPresageAdapter'
    pod 'AppLovinMediationByteDanceAdapter'
    pod 'AppLovinMediationSnapAdapter'
    pod 'AppLovinMediationTapjoyAdapter'
    pod 'AppLovinMediationUnityAdsAdapter'
    pod 'AppLovinMediationVungleAdapter'
    '''
                    with codecs.open(path.join(script_dir, 'skAdNetwork.plist'), encoding='utf-8') as f:
                        skAdNetwork = f.read()
                        findReplace(dst, '#IGE_SDK_SKADNETWORK#', skAdNetwork if appConfig.requires.get('igeSdk') else '', ['Info.plist.in'])
                else:
                    findReplace(dst, '#IGE_SDK_SKADNETWORK#', '', ['Info.plist.in'])
                findReplace(dst, '#IGE_SDK_PODS#', pod if appConfig.requires.get('igeSdk') else '', ['Podfile'])
                pod = '''
target 'igeSdk-static' do
    pod 'FBSDKCoreKit'
    pod 'GA-SDK-IOS'
    pod 'AppsFlyerFramework'
    pod 'AppLovinSDK'
end
    '''
                findReplace(dst, '#IGE_SDK_TARGET#', pod if appConfig.requires.get('igeSdk') else '', ['Podfile'])
                iosConfigLoaded = True
    except:
        pass

    if not androidConfigLoaded:
        findReplace(dst, '#FACEBOOK_APP_ID#', "123456789012345", ['strings.xml'])
        findReplace(dst, '#FACEBOOK_CLIENT_TOKEN#', "1234567890123456789012", ['strings.xml'])
        findReplace(dst, '#GAD_APP_ID#', "ca-app-pub-3940256099942544~3347511713", ['strings.xml'])
        findReplace(dst, '#GGAME_APP_ID#', "123456789012", ['strings.xml'])
        findReplace(dst, '#IGE_SDK_GRADLE_IMPL#', '', ['build.gradle'])
        findReplace(dst, '#IGE_SDK_GRADLE_REPOS#', '', ['build.gradle'])
        safeRemove(path.join(dst, 'app/src/main/java/net/indigames/lib/AppLovinMax.java'))

    if not iosConfigLoaded:
        findReplace(dst, '#FACEBOOK_APP_ID#', "123456789012345", ['Info.plist.in'])
        findReplace(dst, '#FACEBOOK_CLIENT_TOKEN#', "1234567890123456789012", ['Info.plist.in'])
        findReplace(dst, '#GAD_APP_ID#', "ca-app-pub-3940256099942544~3347511713", ['Info.plist.in'])
        findReplace(dst, '#GGAME_APP_ID#', "123456789012", ['Info.plist.in'])
        findReplace(dst, '#APPSTORE_APP_ID#', '123456789012', ['Info.plist.in'])
        findReplace(dst, '#IGE_SDK_PODS#', '', ['Podfile'])

    # Debuggable build
    findReplace(dst, '#DEBUG_BUILD#', '1' if debug else '0', ['CMakeLists.txt'])
    findReplace(dst, '#USE_IGESDK#', '1' if appConfig.requires.get('igeSdk') else '0', ['ImportModules.cmake'])

    if verbose:
        print('Processing DONE!')

def genProject(platform):
    cloneFromTemplate(platform)
    processProject(platform)
    setAppEnvs(platform)
    toggleCMakePackage(platform)

def usage():
    print('build.py')
    print('    -h|--help : show help menu')
    print('    -i|--input : path to project or ROM.zip')
    print('    -p|--platform <windows|android|ios|macos|all> : generated platform')
    print('    -o|--output : output dir')
    print('    -v|--verbose : verbose')
    print('    -d|--debug : generate full source project (development only)')

def main(argv):
    global verbose
    global debug
    global output_dir
    global gen_windows
    global gen_android
    global gen_ios
    global gen_macos
    global gen_emscripten
    global rom_dir
    global appConfig

    try:
        opts, args = getopt.getopt(argv, "hi:o:p:vd", ["help", "input=", "output=", "platform=", "verbose", "debug"])
    except getopt.GetoptError:
        usage()
        sys.exit(2)

    for opt, arg in opts:
        if opt in ("-h", "--help"):
            usage()
            sys.exit()
        elif opt in ("-i", "--input"):
            rom_dir = arg
        elif opt in ("-p", "--platform"):
            if arg == 'windows':
                gen_windows = True
            elif arg == 'android':
                gen_android = True
            elif arg == 'ios':
                gen_ios = True
            elif arg == 'macos':
                gen_macos = True
            elif arg == 'all':
                if os.name == 'nt':
                    gen_windows = True
                else:
                    gen_ios = True
                    gen_macos = True
                gen_android = True
                gen_emscripten = True                    
        elif opt in ("-o", "--output"):
            if path.isabs(r'{}'.format(arg)):
                output_dir = arg
            else:
                output_dir = path.abspath(path.join(os.getcwd(), '{}'.format(arg)))
        elif opt in ("-v", "--verbose"):
            verbose = True
        elif opt in ("-d", "--debug"):
            debug = True

    if verbose:
        print('Rom dir: {}'.format(rom_dir))
        print('Output dir: {}'.format(output_dir))

    if os.path.isdir(rom_dir):
        if not path.exists(path.join('rom', 'config', 'app.yaml')):
            build_rom.build(rom_dir, 'rom')
    elif rom_dir.lower().endswith('.zip'):
        with zipfile.ZipFile(rom_dir, 'r') as zip_ref:
            zip_ref.extractall('rom')    
    rom_dir = 'rom'

    appConfig.readConfig(path.join(rom_dir, 'config', 'app.yaml'))

    if gen_windows:
        genProject('windows')
        project_dir = os.path.join(output_dir, 'windows')
        build_dir = os.path.join(project_dir, 'build')
        safeRemove(build_dir)
        os.makedirs(build_dir)
        os.chdir(build_dir)
        os.system(f'cmake {project_dir} -DCMAKE_BUILD_TYPE=Release -A X64')
        os.system('cmake --build . --config Release --target install -- -m')
    if gen_android:
        genProject('android')
        project_dir = os.path.join(output_dir, 'android')
        os.chdir(project_dir)
        os.system('gradlew.bat assembleRelease')
    if gen_ios:
        genProject('ios')

    if gen_macos:
        genProject('macos')

if __name__ == "__main__":
    main(sys.argv[1:])
