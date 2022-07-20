package net.indigames.lib;

import android.Manifest;
import android.app.Activity;
import android.content.ContentValues;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.ConnectivityManager;
import android.net.Uri;
import android.os.Environment;
import android.provider.MediaStore;
import android.provider.Settings;
import android.util.Log;

import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.core.content.FileProvider;

import org.libsdl.app.SDLActivity;

import java.io.File;
import java.io.IOException;
import java.io.OutputStream;

public class PALHelper
{
    private static final String LOG_TAG = "PAL";

    public static native void NotifyCallback(String eventName, String notifyType, String id);
    public static native void DownloadCallback(String name, int result, String detail);

    public static String LOCAL_NOTIFICATION_INTENT= "message_id";
    public static String FIREBASE_NOTIFICATION_INTENT= "google.message_id";
    public static String NOTIFICATION_RESUME= "pn_resume";
    public static String NOTIFICATION_LAUNCH= "pn_launch";
    public static String LOCAL_NOTIFICATION= "local";
    public static String FIREBASE_NOTIFICATION= "firebase_cloud";

    public static String imgNameSave;

    public static final int MY_PERMISSIONS_REQUEST_READ_EXTERNAL_STORAGE = 1230;
    public static final int MY_PERMISSIONS_REQUEST_WRITE_EXTERNAL_STORAGE = 1231;
    public static final int MY_PERMISSIONS_REQUEST_CAMERA_PERMISSION = 1232;

    public static final int PERMISSIONS_REQUEST_CAMERA = 1;
    public static final int PERMISSIONS_REQUEST_STORAGE = 2;

    public static void NotifyCheck(Intent intent, String action)
    {
        if (intent.getExtras() != null) {
            for (String key : intent.getExtras().keySet()) {
                if(key.contains(FIREBASE_NOTIFICATION_INTENT))
                {
                    String value = intent.getStringExtra(key);
                    NotifyCallback(action, FIREBASE_NOTIFICATION, value);
                    break;
                }
                else if(key.contains(LOCAL_NOTIFICATION_INTENT))
                {
                    String value = intent.getStringExtra(key);
                    NotifyCallback(action, LOCAL_NOTIFICATION, value);
                    break;
                }
            }
        }
    }

    public static boolean checkThenRequestPermission(final Context context, final String permission, final int requestCode)
    {
        int currentAPIVersion = android.os.Build.VERSION.SDK_INT;
        if (currentAPIVersion >= android.os.Build.VERSION_CODES.M) {
            if (ContextCompat.checkSelfPermission(SDLActivity.getContext(), permission) != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions((Activity) context, new String[]{permission}, requestCode);
                return false;
            }
        }
        return true;
    }
    
    public static boolean openAppSettings()
    {
        Intent intent = new Intent(Settings.ACTION_APPLICATION_DETAILS_SETTINGS);
        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        Uri uri = Uri.fromParts("package", SDLActivity.getContext().getPackageName(), null);
        intent.setData(uri);
        SDLActivity.getContext().startActivity(intent);
        return true;
    }

    public static int checkPermission(final int type)
    {
        String permission = "";
        int currentAPIVersion = android.os.Build.VERSION.SDK_INT;
        if (currentAPIVersion >= android.os.Build.VERSION_CODES.M) {
            switch (type)
            {
                case PERMISSIONS_REQUEST_CAMERA:
                    permission = Manifest.permission.CAMERA;
                    break;
                case PERMISSIONS_REQUEST_STORAGE:
                    permission = Manifest.permission.WRITE_EXTERNAL_STORAGE;
                    break;
            }

            // If granted, return 1
            if (ContextCompat.checkSelfPermission(SDLActivity.getContext(), permission) == PackageManager.PERMISSION_GRANTED)
            {
                return 1;
            }

            // If user denied permanently, return -1
            if(!ActivityCompat.shouldShowRequestPermissionRationale((Activity) SDLActivity.getContext(), permission))
            {
                return -1;
            }

            // Never asked, return 0
            return 0;
        }

        // Android L: always accept permissions in manifest
        return 1;
    }

    public static void AddImageToGallery()
    {
        AddImageToGallery(imgNameSave);
    }

    @SuppressWarnings("deprecation")
    public static void AddImageToGallery(final String value)
    {
        imgNameSave = value;

        if (checkThenRequestPermission(SDLActivity.getContext(), Manifest.permission.WRITE_EXTERNAL_STORAGE, MY_PERMISSIONS_REQUEST_WRITE_EXTERNAL_STORAGE))
        {
            String imgName = CoreHelper.getApplicationName(SDLActivity.getContext()) + "_" + System.currentTimeMillis();

            //File image = new File(SDLActivity.getContext().getDir("data", Context.MODE_PRIVATE).getAbsolutePath() + File.separator + value);
            File image = new File(value);
            Bitmap bitmap = BitmapFactory.decodeFile(image.getAbsolutePath());

            try {
                File gallery = new File(SDLActivity.getContext().getExternalFilesDir(Environment.DIRECTORY_PICTURES), imgName);

                Uri uri = FileProvider.getUriForFile(
                        SDLActivity.getContext(),
                        SDLActivity.getContext().getPackageName() + ".provider",
                        gallery);

                Uri externalContentUri = MediaStore.Images.Media.EXTERNAL_CONTENT_URI;
                OutputStream outputStream = SDLActivity.getContext().getContentResolver().openOutputStream(uri);

                bitmap.compress(Bitmap.CompressFormat.JPEG, 100, outputStream);
                try {
                    outputStream.flush();
                    outputStream.close();
                } catch (IOException e) {
                    Log.i(LOG_TAG, e.getMessage());
                }

                ContentValues values = new ContentValues();
                values.put(MediaStore.MediaColumns.TITLE, imgName);
                values.put(MediaStore.MediaColumns.DISPLAY_NAME, imgName);
                values.put(MediaStore.MediaColumns.DATE_ADDED, System.currentTimeMillis() / 1000);
                values.put(MediaStore.MediaColumns.DATE_TAKEN, System.currentTimeMillis());
                values.put(MediaStore.MediaColumns.DATA, gallery.getAbsolutePath());

                SDLActivity.getContext().getContentResolver().insert(externalContentUri, values);
                DownloadResult(value, 1, "");
            } catch (Exception e) {
                e.getStackTrace();
                DownloadResult(value, 0, e.getMessage());
            }
        }
    }

    public static void DownloadResult(String name, int result, String detail)
    {
        DownloadCallback(name, result, detail);
    }

    public static void DownloadResult(int result, String detail)
    {
        DownloadCallback(imgNameSave, result, detail);
    }

    public static boolean isInternetAvailable() {
        ConnectivityManager cm = (ConnectivityManager) SDLActivity.getContext().getSystemService(Context.CONNECTIVITY_SERVICE);

        return cm.getActiveNetworkInfo() != null && cm.getActiveNetworkInfo().isConnected();
    }
}
