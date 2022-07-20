package net.indigames.lib.webview;

import android.annotation.SuppressLint;
import android.content.Context;
import android.webkit.WebChromeClient;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.RelativeLayout;
import net.indigames.lib.WebViewHelper;

public class IGEWebView extends WebView {
    private static final String TAG = WebViewHelper.class.getSimpleName();

    private int mViewTag;

    public IGEWebView(Context context) {
        this(context, -1);
    }

    @SuppressLint("SetJavaScriptEnabled")
    public IGEWebView(Context context, int viewTag) {
        super(context);
        this.mViewTag = viewTag;

        this.setWebViewClient(new WebViewClient());
        this.setWebChromeClient(new WebChromeClient());
    }

    public void setWebViewRect(int left, int top, int maxWidth, int maxHeight) {
        RelativeLayout.LayoutParams layoutParams = new RelativeLayout.LayoutParams(RelativeLayout.LayoutParams.WRAP_CONTENT,
                RelativeLayout.LayoutParams.WRAP_CONTENT);
        layoutParams.leftMargin = left;
        layoutParams.topMargin = top;
        if(maxWidth == 0 && maxHeight == 0)
        {
            layoutParams.addRule(RelativeLayout.CENTER_IN_PARENT);
        }
        else
        {
            layoutParams.width = maxWidth;
            layoutParams.height = maxHeight;
            layoutParams.addRule(RelativeLayout.ALIGN_TOP | RelativeLayout.ALIGN_LEFT);
        }
        this.setLayoutParams(layoutParams);
    }
}
