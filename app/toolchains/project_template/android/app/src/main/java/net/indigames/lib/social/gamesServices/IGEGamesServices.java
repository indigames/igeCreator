package net.indigames.lib.social.gamesServices;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Intent;

import androidx.annotation.NonNull;

import com.google.android.gms.auth.api.signin.GoogleSignIn;
import com.google.android.gms.auth.api.signin.GoogleSignInAccount;
import com.google.android.gms.auth.api.signin.GoogleSignInClient;
import com.google.android.gms.auth.api.signin.GoogleSignInOptions;
import com.google.android.gms.common.api.ApiException;
import com.google.android.gms.games.AchievementsClient;
import com.google.android.gms.games.Games;
import com.google.android.gms.games.LeaderboardsClient;
import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.OnFailureListener;
import com.google.android.gms.tasks.OnSuccessListener;
import com.google.android.gms.tasks.Task;

import net.indigames.lib.SocialHelper;

public class IGEGamesServices
{
    // Client used to sign in with Google APIs
    private GoogleSignInClient mGoogleSignInClient;

    // Client variables
    private AchievementsClient mAchievementsClient = null;
    private LeaderboardsClient mLeaderboardsClient = null;

    // request codes we use when invoking an external activity
    private static final int RC_UNUSED = 5001;
    private static final int RC_SIGN_IN = 9001;

    public void Init(Activity activity)
    {
        SocialHelper.logCallback();

        if(mGoogleSignInClient == null)
        {
            mGoogleSignInClient = GoogleSignIn.getClient(activity, new GoogleSignInOptions.Builder(GoogleSignInOptions.DEFAULT_GAMES_SIGN_IN).build());
        }
        signInSilently(activity);
    }

    public void Release()
    {
        SocialHelper.logCallback();

        onDisconnected();
        mGoogleSignInClient = null;
    }
    
    public void SignIn(Activity activity)
    {
        SocialHelper.logCallback();

        activity.startActivityForResult(mGoogleSignInClient.getSignInIntent(), RC_SIGN_IN);
    }

    private void signInSilently(Activity activity)
    {
        SocialHelper.logCallback();

        mGoogleSignInClient.silentSignIn();
    }
    
    public void SignOut(Activity activity)
    {
        SocialHelper.logCallback();
        onConnected(activity);

        if (!IsSignedIn(activity) || mGoogleSignInClient == null) {
            SocialHelper.logCallback("signOut() called, but was not signed in!");
            return;
        }
        mGoogleSignInClient.signOut().addOnCompleteListener(activity,
                new OnCompleteListener<Void>() {
                    @Override
                    public void onComplete(@NonNull Task<Void> task) {
                        boolean successful = task.isSuccessful();
                        SocialHelper.logCallback("signOut(): " + (successful ? "success" : "failed"));

                        onDisconnected();
                    }
                });
    }

    public boolean IsSignedIn(Activity activity)
    {
        boolean result = GoogleSignIn.getLastSignedInAccount(activity) != null;
        SocialHelper.logCallback("signedIn = " + result);
        return result;
    }

    public void ShowLeaderboard(final Activity activity)
    {
        SocialHelper.logCallback();
        onConnected(activity);

        if(mLeaderboardsClient != null) {
            mLeaderboardsClient.getAllLeaderboardsIntent()
                .addOnSuccessListener(new OnSuccessListener<Intent>() {
                    @Override
                    public void onSuccess(final Intent intent) {
                        SocialHelper.logCallback("ShowLeaderboard : onSuccess");
                        activity.startActivityForResult(intent, RC_UNUSED);
                    }
                })
                .addOnFailureListener(new OnFailureListener() {
                    @Override
                    public void onFailure(@NonNull Exception e) {
                        handleException(activity, e);
                    }
                });
        }
    }

    public void ShowLeaderboard(final String id,  final Activity activity)
    {
        SocialHelper.logCallback();
        onConnected(activity);

        if(mLeaderboardsClient != null) {
            mLeaderboardsClient.getLeaderboardIntent(id)
                    .addOnSuccessListener(new OnSuccessListener<Intent>() {
                        @Override
                        public void onSuccess(final Intent intent) {
                            SocialHelper.logCallback("ShowLeaderboard : onSuccess");
                            activity.startActivityForResult(intent, RC_UNUSED);
                        }
                    })
                    .addOnFailureListener(new OnFailureListener() {
                        @Override
                        public void onFailure(@NonNull Exception e) {
                            handleException(activity, e);
                        }
                    });
        }
    }

    public void SubmitScoreLeaderboard(final String id, final int value, final Activity activity)
    {
        SocialHelper.logCallback();
        onConnected(activity);

        if(mLeaderboardsClient != null && value >= 0) {
            mLeaderboardsClient.submitScore(id, value);
        }
    }

    public void ShowAchievement(final Activity activity)
    {
        SocialHelper.logCallback();
        onConnected(activity);

        if(mAchievementsClient != null) {
            mAchievementsClient.getAchievementsIntent()
                    .addOnSuccessListener(new OnSuccessListener<Intent>() {
                        @Override
                        public void onSuccess(Intent intent) {
                            activity.startActivityForResult(intent, RC_UNUSED);
                        }
                    })
                    .addOnFailureListener(new OnFailureListener() {
                        @Override
                        public void onFailure(@androidx.annotation.NonNull Exception e) {
                            handleException(activity, e);
                        }
                    });
        }
    }

    public void UnlockAchievement(final String id, final Activity activity)
    {
        SocialHelper.logCallback();
        onConnected(activity);

        if(mAchievementsClient != null) {
            mAchievementsClient.unlock(id);
        }
    }

    public void IncrementAchievement(final String id, final int value, final Activity activity)
    {
        SocialHelper.logCallback();
        onConnected(activity);

        if(mAchievementsClient != null && value > 0) {
            mAchievementsClient.increment(id, value);
        }
    }

    private void onConnected(Activity activity) {
        if(mAchievementsClient == null || mLeaderboardsClient == null)
        {
            GoogleSignInAccount account = GoogleSignIn.getLastSignedInAccount(activity);
            if(account != null) {
                if (mAchievementsClient == null) {
                    mAchievementsClient = Games.getAchievementsClient(activity, account);
                }

                if (mLeaderboardsClient == null) {
                    mLeaderboardsClient = Games.getLeaderboardsClient(activity, account);
                }
            }
        }
    }

    private void onDisconnected() {
        SocialHelper.logCallback();
        mAchievementsClient = null;
        mLeaderboardsClient = null;
    }

    private void handleException(Activity activity, Exception e) {
        int status = 0;

        if (e instanceof ApiException) {
            ApiException apiException = (ApiException) e;
            status = apiException.getStatusCode();
        }

        String message = "{e}";

        new AlertDialog.Builder(activity)
                .setMessage(message)
                .setNeutralButton(android.R.string.ok, null)
                .show();
    }
}




