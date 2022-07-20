package #PACKAGE_NAME#;

import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.view.View;
import android.view.WindowInsets;
import android.view.WindowInsetsController;

import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;

import org.libsdl.app.SDLActivity;
import net.indigames.lib.*;

public final class MainActivity extends SDLActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        StorageManager storageManager = new StorageManager((Context)this, "data", false);
        storageManager.initData();

        PALHelper.NotifyCheck(getIntent(), PALHelper.NOTIFICATION_LAUNCH);
        try {
            CoreHelper.layoutClassVariable = Class.forName("#PACKAGE_NAME#.R$layout");
            CoreHelper.idClassVariable = Class.forName("#PACKAGE_NAME#.R$id");
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (hasFocus) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
                getWindow().setDecorFitsSystemWindows(false);
                if (getWindow().getInsetsController() != null) {
                    getWindow().getInsetsController().hide(WindowInsets.Type.statusBars() | WindowInsets.Type.navigationBars());
                    getWindow().getInsetsController().setSystemBarsBehavior(WindowInsetsController.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE);
                }
            } else {
                getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
            }
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    @Override
    protected String[] getLibraries() {
        List<String> libs = new ArrayList<>();
        libs.add("#TARGET_NAME#");
        for(String lib: super.getLibraries()) {
            libs.add(lib);
        }
        return libs.toArray(new String[0]);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        try {
            Class clazz = Class.forName("net.indigames.lib.SocialHelper");
            Method m = clazz.getMethod("activityResult");
            m.invoke(requestCode, resultCode, data);
        } catch (Exception var6) {
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        if(requestCode == PALHelper.MY_PERMISSIONS_REQUEST_WRITE_EXTERNAL_STORAGE) {
            if (grantResults[0] == PackageManager.PERMISSION_GRANTED && grantResults.length > 0 ) {
                PALHelper.AddImageToGallery();
            }
        }
        else {
            super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        }
    }

    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        try {
            Intent message = new Intent((Context)this, Class.forName("com.google.firebase.messaging.MessageForwardingService"));
            message.setAction("com.google.android.c2dm.intent.RECEIVE");
            message.putExtras(intent);
            message.setData(intent.getData());
            this.startService(message);
        } catch (Exception ex) {
        }
        PALHelper.NotifyCheck(intent, PALHelper.NOTIFICATION_RESUME);
    }
}
