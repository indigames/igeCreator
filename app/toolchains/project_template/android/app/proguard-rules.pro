# Add project specific ProGuard rules here.
# You can control the set of applied configuration files using the
# proguardFiles setting in build.gradle.
#
# For more details, see
#   http://developer.android.com/guide/developing/tools/proguard.html

# If your project uses WebView with JS, uncomment the following
# and specify the fully qualified class name to the JavaScript interface
# class:
#-keepclassmembers class fqcn.of.javascript.interface.for.webview {
#   public *;
#}

# Uncomment this to preserve the line number information for
# debugging stack traces.
#-keepattributes SourceFile,LineNumberTable

# If you keep the line number information, uncomment this to
# hide the original source file name.
#-renamesourcefileattribute SourceFile

# GoogleAds
-keep class com.google.android.gms.common.ConnectionResult {
    int SUCCESS;
}
-keep class com.google.android.gms.ads.identifier.AdvertisingIdClient {
    com.google.android.gms.ads.identifier.AdvertisingIdClient$Info getAdvertisingIdInfo(android.content.Context);
}
-keep class com.google.android.gms.ads.identifier.AdvertisingIdClient$Info {
    java.lang.String getId();
    boolean isLimitAdTrackingEnabled();
}
-keep public class com.android.installreferrer.** { *; }

# Internal classes
-keep public class net.indigames.lib.** { public *; }
-keep public class org.libsdl.app.** { public *; }

# Adjust
-keep class com.adjust.sdk.** { *; }

# AppsFlyer
-keep class com.appsflyer.** { *; }

# GameAnalytics
-keep class com.gameanalytics.sdk.** { *; }
-keep class com.gameanalytics.sdk.GAPlatform { *; }
-keep class com.gameanalytics.sdk.GAPlatform.** { *; }
-keep class android.net.ConnectivityManager.** { *; }
-keep class com.google.android.instantapps.InstantApps { *; }
-keepclassmembers class com.google.android.instantapps.InstantApps { *; }
