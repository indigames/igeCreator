package net.indigames.lib.notification;

import android.os.Build;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;

import androidx.annotation.NonNull;
import androidx.core.app.NotificationCompat;
import androidx.core.app.NotificationManagerCompat;
import androidx.work.Data;
import androidx.work.Worker;
import androidx.work.WorkerParameters;

import org.libsdl.app.SDLActivity;

public class NotifyWorker extends Worker {
    public NotifyWorker(@NonNull Context context, @NonNull WorkerParameters workerParams) {
        super(context, workerParams);
    }

    @NonNull
    @Override
    public Result doWork() {
        triggerNotification();

        return Result.success();
    }

    private void triggerNotification() {
        Intent intent = SDLActivity.getContext().getPackageManager().getLaunchIntentForPackage(SDLActivity.getContext().getPackageName());
        PendingIntent pIntent = PendingIntent.getActivity(getApplicationContext(), 1, intent, PendingIntent.FLAG_UPDATE_CURRENT);

        //get latest event details
        Data inputData = getInputData();
        int id = inputData.getInt(NotifyEvent.EventIDTag, 1);
        String title = inputData.getString(NotifyEvent.EventTitleTag);
        String message = inputData.getString(NotifyEvent.EventMessageTag);
        int prio = inputData.getInt(NotifyEvent.EventPriorityTag, NotificationCompat.PRIORITY_DEFAULT);
        int icon_id = SDLActivity.getContext().getResources().getIdentifier("ic_notification", "drawable", SDLActivity.getContext().getPackageName());
        String channelId = "IGE_GameNotify";

        //build the notification
        NotificationCompat.Builder notificationBuilder =
                new NotificationCompat.Builder(getApplicationContext(), channelId)
                        .setSmallIcon(icon_id)
                        .setContentTitle(title)
                        .setContentText(message)
                        .setStyle(new NotificationCompat.BigTextStyle()
                                .setBigContentTitle(title)
                                .setSummaryText(message))
                        .setContentIntent(pIntent)
                        .setPriority(prio)
                        .setAutoCancel(true);

        //trigger the notification
        NotificationManagerCompat notificationManager =
                NotificationManagerCompat.from(getApplicationContext());

        // set channel on Android 8+
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O)
        {
            NotificationChannel channel = new NotificationChannel(
                    channelId,
                    "Game notification channel",
                    NotificationManager.IMPORTANCE_HIGH);
            notificationManager.createNotificationChannel(channel);
            notificationBuilder.setChannelId(channelId);
        }

        //we give each notification the ID of the event it's describing,
        //to ensure they all show up and there are no duplicates
        notificationManager.notify(id, notificationBuilder.build());
    }
}
