package net.indigames.lib;

import android.content.Intent;
import org.libsdl.app.SDLActivity;
import net.indigames.lib.webview.IGEWebActivity;

public class WebViewHelper {
    private static final String TAG = "IGEWebView";
    private static int viewTag = 0;

    public static int createWebView() {
        return viewTag;
    }

    public static void removeWebView() {
    }

    public static void setVisible(final boolean visible) {
    }

    public static void setWebViewRect(final int left, final int top, final int maxWidth, final int maxHeight) {
    }

    public static void loadUrl(final String url, final boolean cleanCachedData) {
        IGEWebActivity.url = url;

        SDLActivity activity = (SDLActivity) SDLActivity.getContext();
        activity.startActivity(new Intent(activity, IGEWebActivity.class));
    }
}
