package net.indigames.lib.social.gamesSharing;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.widget.Toast;

import androidx.core.content.FileProvider;
import net.indigames.lib.SocialHelper;

import java.io.File;

public class IGEInstagramSharing
{
    private final String appPackageName = "com.instagram.android";

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
    }

    private void SharePhoto(final String value, final Activity activity)
    {
        SocialHelper.logCallback(activity.getPackageName());
        File image = new File(activity.getDir("data", Context.MODE_PRIVATE).getAbsolutePath() + File.separator + value);
        Uri uri = FileProvider.getUriForFile(
                activity,
                activity.getPackageName() + ".provider",
                image);

        //activity.startActivity(Intent.createChooser(share, "Share to"));
        if(Available(activity))
        {
            Intent share = new Intent(Intent.ACTION_SEND);
            share.setType("image/*");
            share.putExtra(Intent.EXTRA_STREAM, uri);
            share.setPackage(appPackageName);

            activity.startActivity(share);
        } else {
            Toast.makeText(activity, "Instagram app isn't found", Toast.LENGTH_LONG).show();
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




