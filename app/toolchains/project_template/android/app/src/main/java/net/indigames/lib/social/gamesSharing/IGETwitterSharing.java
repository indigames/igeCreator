package net.indigames.lib.social.gamesSharing;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.widget.Toast;

import androidx.core.content.FileProvider;
import java.io.File;

import net.indigames.lib.SocialHelper;

public class IGETwitterSharing
{
    private final String appPackageName = "com.twitter.android";

    public void Init(Activity activity)
    {
        SocialHelper.logCallback();
    }

    public void Release()
    {
        SocialHelper.logCallback();
    }

    public void SignIn(Activity activity)
    {
        SocialHelper.logCallback();
    }

    public void SignOut(Activity activity)
    {
        SocialHelper.logCallback();
    }

    public boolean IsSignedIn(Activity activity)
    {
        SocialHelper.logCallback();

        return false;
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

        if(SocialHelper.ClientAppAvailable(activity, appPackageName)) {
            Intent share = new Intent(Intent.ACTION_SEND);
            share.setType("text/plain");
            share.putExtra(Intent.EXTRA_TEXT, value);
            share.setPackage(appPackageName);

            activity.startActivity(share);
        }
        else {
            Toast.makeText(activity, "Twitter app isn't found", Toast.LENGTH_LONG).show();
        }
    }

    private void SharePhoto(final String value, final Activity activity)
    {
        SocialHelper.logCallback(activity.getPackageName());
        File image = new File(activity.getDir("data", Context.MODE_PRIVATE).getAbsolutePath() + File.separator + value);
        Uri uri = FileProvider.getUriForFile(
                activity,
                activity.getPackageName() + ".provider",
                image);

        if(Available(activity)) {
            Intent share = new Intent(Intent.ACTION_SEND);
            share.setType("image/*");
            share.putExtra(Intent.EXTRA_STREAM, uri);
            share.setPackage(appPackageName);

            activity.startActivity(share);
        } else {
            Toast.makeText(activity, "Twitter app isn't found", Toast.LENGTH_LONG).show();
        }

    }

    public String GetProfileID()
    {
        SocialHelper.logCallback();
        return "";
    }

    public String GetProfileName()
    {
        SocialHelper.logCallback();
        return "";
    }

    public boolean Available(final Activity activity)
    {
        return SocialHelper.ClientAppAvailable(activity, appPackageName);
    }
}




