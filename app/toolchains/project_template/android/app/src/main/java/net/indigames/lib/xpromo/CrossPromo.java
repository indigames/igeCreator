package net.indigames.lib.xpromo;

import android.app.Activity;
import android.util.Log;

import com.ketchapp.promotion.GDPROptInStatus;
import com.ketchapp.promotion.GdprHelper;
import com.ketchapp.promotion.Promotion;
import com.ketchapp.promotion.PromotionActivity;
import com.ketchapp.promotion.PromotionInterstitial;
import com.ketchapp.promotion.PromotionSquare;
import com.ketchapp.promotion.Square;
import com.ketchapp.promotion.SquareHelper;
import com.ketchapp.promotion.URLs;
import com.ketchapp.promotion.Unity3d.BoolResultCallback;
import com.ketchapp.promotion.Unity3d.EventResultCallback;
import com.ketchapp.promotion.Utils;

import org.libsdl.app.SDLActivity;

public class CrossPromo {
    private static final String LOG_TAG = "CrossPromo";
    private static boolean s_bInitialized = false;

    private enum XPromoEvent {
        GDPR(0),
        PROMO(1),
        SQUARE(2);
        XPromoEvent(final int val) { value = val; }
        public int getValue() { return value; }
        private final int value;
    }

    public static void initialize() {
        if(!s_bInitialized) {
            GdprHelper.getInstance().setActivity((Activity)SDLActivity.getContext());
            GdprHelper.getInstance().getInitialData();
            fetchSquare(); // cache square promo
            s_bInitialized = true;
        }
    }

    /**
     * Check if the user is in GDPR country
     */
    public static boolean isInGDPRCountry() {
        return s_bInitialized ? GdprHelper.getInstance().IsFirstPopUpAvailable() : false;
    }

    /**
     * Return int values: (0) GDPR has been accepted (optin), (1) GDPR has been refused (optout), (2) GDPR first run
     */
    public static int getGdprValue() {
        return s_bInitialized ? GdprHelper.getInstance().IsKetchappGdprOptin().GetValue() : 0;
    }

    /**
     * Show GDPR dialog
     */
    public static void showGdpr() {
        if(s_bInitialized) {
            GdprHelper.getInstance().OpenKetchappGdprWindow(new BoolResultCallback() {
                @Override
                public void OnResult(boolean result) {
                    Log.d(LOG_TAG, "OpenKetchappGdprWindow - result:" + result);
                    forwardEvent(XPromoEvent.GDPR.getValue(), result ? 0 : -1, "");
                }
            });
        }
        else {
            Log.d(LOG_TAG, "showGdpr failed - not initialized");
        }
    }

    /**
     * Load square promotion
     */
    public static void fetchSquare() {
        if(s_bInitialized) {
            SquareHelper.GetInstance().SetActivity((Activity)SDLActivity.getContext());
            SquareHelper.GetInstance().FetchSquare((Activity)SDLActivity.getContext(), new Square.OnSquareLoadedCallback() {
                @Override
                public void OnSquareLoaded(SquareLoadResult loadResult) {
                    Log.d(LOG_TAG, "FetchSquare - EventResultCallback - OnResult "+ loadResult);
                    forwardEvent(XPromoEvent.SQUARE.getValue(), loadResult == Square.OnSquareLoadedCallback.SquareLoadResult.LOAD_FAILED ? -1 : 0, "");
                }
            });
        }
        else {
            Log.d(LOG_TAG, "fetchSquare failed - not initialized");
        }
    }

    /**
     * Show square promotion
     */
    public static void showSquare(final float x, final float y, final int width, final int height, final boolean normalized ) {
        if(s_bInitialized) {
            SquareHelper.GetInstance().SetSquareTransform(x, y, width, height, normalized);
            SquareHelper.GetInstance().ShowSquare(new EventResultCallback() {
                @Override
                public void OnResult(String result) {
                    Log.d(LOG_TAG, "Square result - EventResultCallback - OnResult "+ result);
                    forwardEvent(XPromoEvent.SQUARE.getValue(), 1, result);
                }
            });
        }
        else {
            Log.d(LOG_TAG, "showSquare failed - not initialized");
        }
    }

    /**
     * Hide and destroy square promotion
     */
    public static void destroySquare() {
        if(s_bInitialized) {
            SquareHelper.GetInstance().DestroySquare();
        }
    }

    /**
     * Show cross-promotion interstial
     */
    public static void showInterstitial() {
        if(s_bInitialized) {
            final Activity activity = (Activity)SDLActivity.getContext();
            final String url = (new StringBuilder()).append(URLs.CAMPAIGN_LINK_INTERSTITIAL).append(Utils.getPackageName(activity)).toString();
            PromotionInterstitial.loadAndShow(activity, url, new Promotion.PromotionLoadCallback() {
                @Override
                public void OnPromotionLoaded(boolean hasVideo) {
                    Log.d(LOG_TAG, "loadAndShow - onPromotionLoaded" + hasVideo);
                    forwardEvent(XPromoEvent.PROMO.getValue(), 0, "");
                }

                @Override
                public void OnPromotionFailedToLoad(PromotionLoadError error) {
                    Log.d(LOG_TAG, "loadAndShow - OnPromotionFailedToLoad" + error.toString());
                    forwardEvent(XPromoEvent.PROMO.getValue(), -1, error.toString());
                }
            }, () -> {
                Log.d(LOG_TAG, "loadAndShow - PromotionClosedCallback - OnCallback");
                forwardEvent(XPromoEvent.PROMO.getValue(), 1, "");
            },null);
                }
        else {
            Log.d(LOG_TAG, "showInterstitial failed - not initialized");
        }
    }


    /**
     * Forward reesult to C++ using JNI Bridge
     */
    private static void forwardEvent(final int event, final int status, final String msg) {
        nativeForwardEvent(event, status, msg);
    }

    private static native void nativeForwardEvent(final int event, final int status, final String msg);
}
