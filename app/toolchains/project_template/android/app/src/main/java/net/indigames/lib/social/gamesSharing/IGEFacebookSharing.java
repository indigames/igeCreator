package net.indigames.lib.social.gamesSharing;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Environment;
import android.util.Log;

import com.facebook.AccessToken;
import com.facebook.CallbackManager;
import com.facebook.FacebookCallback;
import com.facebook.FacebookException;
import com.facebook.Profile;
import com.facebook.ProfileTracker;
import com.facebook.login.LoginResult;
import com.facebook.share.ShareApi;
import com.facebook.share.Sharer;
import com.facebook.share.model.ShareLinkContent;
import com.facebook.share.model.SharePhoto;
import com.facebook.share.model.SharePhotoContent;
import com.facebook.share.widget.ShareDialog;
import com.facebook.login.LoginManager;

import net.indigames.lib.SocialHelper;

import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;

public class IGEFacebookSharing
{
    private ShareDialog shareDialog;
    private boolean canPresentShareDialog;

    private ProfileTracker profileTracker;
    private CallbackManager callbackManager;

    private FacebookCallback<Sharer.Result> shareCallback = new FacebookCallback<Sharer.Result>() {
        @Override
        public void onCancel() {
            Log.d("HelloFacebook", "Canceled");
        }

        @Override
        public void onError(FacebookException error) {
            Log.d("HelloFacebook", String.format("Error: %s", error.toString()));
        }

        @Override
        public void onSuccess(Sharer.Result result) {
            Log.d("HelloFacebook", "Success!");
        }
    };

    public void Init(Activity activity)
    {
        SocialHelper.logCallback();

        callbackManager = CallbackManager.Factory.create();

        LoginManager.getInstance().registerCallback(callbackManager,
                new FacebookCallback<LoginResult>() {
                    @Override
                    public void onSuccess(LoginResult loginResult) {
                        SocialHelper.logCallback();
                    }

                    @Override
                    public void onCancel() {
                        SocialHelper.logCallback();
                    }

                    @Override
                    public void onError(FacebookException exception) {
                        SocialHelper.logCallback();
                    }
                });

        shareDialog = new ShareDialog(activity);
        shareDialog.registerCallback(
                callbackManager,
                shareCallback);

        profileTracker = new ProfileTracker() {
            @Override
            protected void onCurrentProfileChanged(Profile oldProfile, Profile currentProfile) {
                SocialHelper.logCallback();
            }
        };
        profileTracker.startTracking();
    }

    public void Release()
    {
        SocialHelper.logCallback();

        profileTracker.stopTracking();
    }

    public void SignIn(Activity activity)
    {
        SocialHelper.logCallback();

        LoginManager.getInstance().logInWithReadPermissions(activity, Arrays.asList("public_profile"));
    }

    public void SignOut(Activity activity)
    {
        SocialHelper.logCallback();
    }

    public boolean IsSignedIn(Activity activity)
    {
        SocialHelper.logCallback();

        AccessToken accessToken = AccessToken.getCurrentAccessToken();
        boolean isLoggedIn = accessToken != null && !accessToken.isExpired();

        return isLoggedIn;
    }

    public void Share(int share, String value, final Activity activity)
    {
        SocialHelper.logCallback();

        switch (share)
        {
            case SocialHelper.ShareType.kLink:
                ShareLink(value, activity);
                break;
            case SocialHelper.ShareType.kPhoto:
                SharePhoto(value, activity);
                break;
        }
    }

    private void ShareLink(final String value, final Activity activity)
    {
        SocialHelper.logCallback(value);

        Profile profile = Profile.getCurrentProfile();
        ShareLinkContent linkContent = new ShareLinkContent.Builder()
                .setContentUrl(Uri.parse(value))
                .build();

        canPresentShareDialog = ShareDialog.canShow(ShareLinkContent.class);

        if (canPresentShareDialog && shareDialog != null) {
            SocialHelper.logCallback("shareDialog");
            shareDialog.show(linkContent);
        } else if (profile != null && hasPublishPermission()) {
            SocialHelper.logCallback("ShareApi");
            ShareApi.share(linkContent, shareCallback);
        }
    }

    private void SharePhoto(final String value, final Activity activity)
    {
        //File image = new File(activity.getExternalFilesDir(null).getAbsolutePath(), value);
        File image = new File(activity.getDir("data", Context.MODE_PRIVATE) + File.separator + value);

        Bitmap bitmap = BitmapFactory.decodeFile(image.getAbsolutePath());

        SharePhoto sharePhoto = new SharePhoto.Builder().setBitmap(bitmap).build();
        ArrayList<SharePhoto> photos = new ArrayList<>();
        photos.add(sharePhoto);

        SharePhotoContent photoContent = new SharePhotoContent.Builder().setPhotos(photos).build();

        canPresentShareDialog = ShareDialog.canShow(SharePhotoContent.class);

        if (canPresentShareDialog && shareDialog != null) {
            shareDialog.show(photoContent);
        } else if (hasPublishPermission()) {
            ShareApi.share(photoContent, shareCallback);
        }
    }

    private boolean hasPublishPermission() {
        return AccessToken.isCurrentAccessTokenActive()
                && AccessToken.getCurrentAccessToken().getPermissions().contains("publish_actions");
    }

    public String GetProfileID()
    {
        SocialHelper.logCallback();

        Profile profile = Profile.getCurrentProfile();
        if(profile != null)
        {
            return profile.getId();
        }
        SocialHelper.logCallback("profile = null");
        return "";
    }

    public String GetProfileName()
    {
        SocialHelper.logCallback();

        Profile profile = Profile.getCurrentProfile();
        if(profile != null)
        {
            return profile.getName();
        }
        SocialHelper.logCallback("profile = null");
        return "";
    }

    public void activityResult(int requestCode, int resultCode, Intent data)
    {
        callbackManager.onActivityResult(requestCode, resultCode, data);
    }

    public boolean Available(final Activity activity)
    {
        return true;    //handle via SDK
    }
}




