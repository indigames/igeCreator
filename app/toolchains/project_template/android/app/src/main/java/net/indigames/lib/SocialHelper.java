package net.indigames.lib;

import android.app.Activity;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.util.Log;

import net.indigames.lib.social.gamesServices.IGEGamesServices;
import net.indigames.lib.social.gamesSharing.IGEFacebookSharing;
import net.indigames.lib.social.gamesSharing.IGEInstagramSharing;
import net.indigames.lib.social.gamesSharing.IGETwitterSharing;

import org.libsdl.app.SDLActivity;
import android.content.Intent;

import java.util.List;

public class SocialHelper
{
    private static final String SOCIAL_LOG_TAG = "Social";

    private static SDLActivity activity = null;

    // gamesServices
    private static IGEGamesServices gamesServices;

    // gamesSharing
    public static class SnsType
    {
        public static final int kFacebook = 0;
        public static final int kInstagram = 1;
        public static final int kTwitter = 2;
    }

    public static class ShareType
    {
        public static final int kLink = 0;
        public static final int kPhoto = 1;
    };
    private static IGEFacebookSharing facebookSharing;
    private static IGEInstagramSharing instagramSharing;
    private static IGETwitterSharing twitterSharing;

    public static void GamesServicesInit()
    {
        if(activity == null)
        {
            activity = (SDLActivity) SDLActivity.getContext();
        }

        gamesServices = new IGEGamesServices();

        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                gamesServices.Init(activity);
            }
        });
    }

    public static void GamesServicesRelease()
    {
        gamesServices.Release();
        gamesServices = null;
    }

    public static void GamesServicesSignIn()
    {
        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                gamesServices.SignIn(activity);
            }
        });
    }

    public static void GamesServicesSignOut()
    {
        gamesServices.SignOut(activity);
    }

    public static boolean GamesServicesIsSignedIn()
    {
        return gamesServices.IsSignedIn(activity);
    }

    public static void GamesServicesShowLeaderboard(final String id)
    {
        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (id.isEmpty()) {
                    gamesServices.ShowLeaderboard(activity);
                }
                else {
                    gamesServices.ShowLeaderboard(id, activity);
                }
            }
        });
    }

    public static void GamesServicesSubmitScoreLeaderboard(final String id, final int value)
    {
        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                gamesServices.SubmitScoreLeaderboard(id, value, activity);
            }
        });
    }

    public static void GamesServicesShowAchievement()
    {
        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                gamesServices.ShowAchievement(activity);
            }
        });
    }

    public static void GamesServicesUnlockAchievement(final String id)
    {
        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                gamesServices.UnlockAchievement(id, activity);
            }
        });
    }

    public static void GamesServicesIncrementAchievement(final String id, final int value)
    {
        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                gamesServices.IncrementAchievement(id, value, activity);
            }
        });
    }

    // region GameSharing
    public static void GamesSharingInit(int sns)
    {
        logCallback();

        if(activity == null)
        {
            activity = (SDLActivity) SDLActivity.getContext();
        }

        switch (sns)
        {
            case SnsType.kFacebook:
            {
                if(facebookSharing == null)
                {
                    facebookSharing = new IGEFacebookSharing();
                    activity.runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            facebookSharing.Init(activity);
                        }
                    });
                }
                break;
            }
            case SnsType.kInstagram:
            {
                if(instagramSharing == null)
                {
                    instagramSharing = new IGEInstagramSharing();
                    activity.runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            instagramSharing.Init(activity);
                        }
                    });
                }
                break;
            }
            case SnsType.kTwitter:
            {
                if(twitterSharing == null)
                {
                    twitterSharing = new IGETwitterSharing();
                    activity.runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            twitterSharing.Init(activity);
                        }
                    });
                }
                break;
            }
        }
    }

    public static void GamesSharingRelease(int sns)
    {
        logCallback();

        switch (sns)
        {
            case SnsType.kFacebook:
            {
                if(facebookSharing != null)
                {
                    facebookSharing = null;
                }
                break;
            }
            case SnsType.kInstagram:
            {
                if(instagramSharing != null)
                {
                    instagramSharing = null;
                }
                break;
            }
            case SnsType.kTwitter:
            {
                if(twitterSharing != null)
                {
                    twitterSharing = null;
                }
                break;
            }
        }
    }

    public static void GamesSharingSignIn(int sns)
    {
        logCallback(String.valueOf(sns));

        switch (sns)
        {
            case SnsType.kFacebook:
            {
                if(facebookSharing != null)
                {
                    activity.runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            facebookSharing.SignIn(activity);
                        }
                    });
                }
                break;
            }
        }
    }

    public static void GamesSharingSignOut(int sns)
    {
        logCallback(String.valueOf(sns));
        switch (sns)
        {
            case SnsType.kFacebook:
            {
                if(facebookSharing != null)
                {
                    activity.runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            facebookSharing.SignOut(activity);
                        }
                    });
                }
                break;
            }
        }
    }

    public static boolean GamesSharingIsSignedIn(int sns)
    {
        logCallback(String.valueOf(sns));

        switch (sns)
        {
            case SnsType.kFacebook:
            {
                if(facebookSharing != null)
                {
                    return facebookSharing.IsSignedIn(activity);
                }
                break;
            }
        }
        return false;
    }

    public static boolean GamesSharingShare(final int sns, final int share, final String value)
    {
        logCallback(sns + " " + share);

        if(!PALHelper.isInternetAvailable())
        {
            return false;
        }
        switch (sns)
        {
            case SnsType.kFacebook:
            {
                if(facebookSharing != null)
                {
                    activity.runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            facebookSharing.Share(share, value, activity);
                        }
                    });
                }
                return true;
            }
            case SnsType.kInstagram:
            {
                if(instagramSharing == null) {
                    instagramSharing = new IGEInstagramSharing();
                }
                activity.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        instagramSharing.Share(share, value, activity);
                    }
                });
                return true;
            }
            case SnsType.kTwitter:
            {
                if(twitterSharing == null) {
                    twitterSharing = new IGETwitterSharing();
                }
                activity.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        twitterSharing.Share(share, value, activity);
                    }
                });
                return true;
            }
        }
        return false;
    }

    public static String GamesSharingGetProfileID(final int sns)
    {
        logCallback();

        switch (sns)
        {
            case SnsType.kFacebook:
            {
                if(facebookSharing != null)
                {
                    logCallback(facebookSharing.GetProfileID());
                    return facebookSharing.GetProfileID();
                }
                break;
            }
        }
        return "";
    }

    public static String GamesSharingGetProfileName(final int sns)
    {
        logCallback();

        switch (sns)
        {
            case SnsType.kFacebook:
            {
                if(facebookSharing != null)
                {
                    logCallback(facebookSharing.GetProfileName());
                    return facebookSharing.GetProfileName();
                }
                break;
            }
        }
        return "";
    }

    public static boolean GamesSharingAvailable(int sns)
    {
        logCallback(String.valueOf(sns));

        switch (sns)
        {
            case SnsType.kFacebook:
            {
                return true;    //handle via sdk
            }
            case SnsType.kInstagram:
            {
                if(instagramSharing == null) {
                    instagramSharing = new IGEInstagramSharing();
                }
                return instagramSharing.Available(activity);
            }
            case SnsType.kTwitter:
            {
                if(twitterSharing == null) {
                    twitterSharing = new IGETwitterSharing();
                }
                return twitterSharing.Available(activity);
            }
        }
        return false;
    }

    public static void activityResult(int requestCode, int resultCode, Intent data)
    {
        if(facebookSharing != null)
        {
            facebookSharing.activityResult(requestCode, resultCode, data);
        }
    }


    public static boolean ClientAppAvailable(Activity activity, String name)
    {
        Intent tweetIntent = new Intent(Intent.ACTION_SEND);
        tweetIntent.setType("image/*");

        PackageManager packManager = activity.getPackageManager();
        List<ResolveInfo> resolvedInfoList = packManager.queryIntentActivities(tweetIntent, PackageManager.MATCH_DEFAULT_ONLY);

        boolean available = false;
        for (ResolveInfo resolveInfo : resolvedInfoList) {
            SocialHelper.logCallback(resolveInfo.activityInfo.packageName);
            if (resolveInfo.activityInfo.packageName.startsWith(name)) {
                available = true;
                break;
            }
        }
        return available;
    }

    public static void logCallback(String error)
    {
        //String callbackName = new Throwable().getStackTrace()[1].getMethodName();
        //Log.i(SOCIAL_LOG_TAG, callbackName + " error = " + error);
    }

    public static void logCallback()
    {
        //String callbackName = new Throwable().getStackTrace()[1].getMethodName();
        //Log.i(SOCIAL_LOG_TAG, callbackName);
    }
}





