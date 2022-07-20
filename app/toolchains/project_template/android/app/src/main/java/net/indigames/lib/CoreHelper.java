package net.indigames.lib;

import org.libsdl.app.SDLActivity;

import android.app.Activity;
import android.content.ContentValues;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.provider.MediaStore;

import android.os.Environment;
import android.util.Log;

import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.core.content.FileProvider;

import java.io.File;
import java.io.IOException;
import java.io.OutputStream;
import java.lang.reflect.Field;
import java.util.Locale;

import android.Manifest.permission;

public class CoreHelper
{
    private static final String LOG_TAG = "Core";

    public static Class layoutClassVariable;
    public static Class idClassVariable;

    public static int getResId(String resourceName, Class<?> Rclass) {
        int id = -1;
        try {
            if (Rclass != null) {
                final Field field = Rclass.getField(resourceName);
                if (field != null)
                    id = field.getInt(null);
            }
        } catch (final Exception e) {
            e.printStackTrace();
        }

        return id;
    }

    public static void logCallback(String error)
    {
        //String callbackName = new Throwable().getStackTrace()[1].getMethodName();
        //Log.i(LOG_TAG, callbackName + " error = " + error);
    }

    public static void logCallback()
    {
        //String callbackName = new Throwable().getStackTrace()[1].getMethodName();
        //Log.i(LOG_TAG, callbackName);
    }

    public static String getApplicationName(Context context) {
        ApplicationInfo applicationInfo = context.getApplicationInfo();
        int stringId = applicationInfo.labelRes;
        return stringId == 0 ? applicationInfo.nonLocalizedLabel.toString() : context.getString(stringId);
    }

    public static String getLanguage() {
        Locale locale = Locale.getDefault();
        return locale.getLanguage();
    }
    public static String getLocale() {
        Locale locale = Locale.getDefault();
        return locale.getCountry();
    }


}
