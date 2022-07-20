package net.indigames.lib.notification;

import android.app.AlarmManager;
import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.media.AudioAttributes;
import android.media.RingtoneManager;
import android.net.Uri;
import android.os.Build;
import android.os.SystemClock;

import static android.app.Notification.DEFAULT_ALL;
import androidx.core.app.NotificationCompat;

import net.indigames.lib.PALHelper;

import org.libsdl.app.SDLActivity;

public class IGENotificationPublisher extends BroadcastReceiver {

    public static String NOTIFICATION_ID = "notification_id";
    public static String NOTIFICATION = "notification";

    public static String NOTIFICATION_CHANNEL = "appName_channel_01";
    public static String NOTIFICATION_NAME = "appName";

    @Override
    public void onReceive(final Context context, Intent intent) {

        NotificationManager notificationManager = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);

        Notification notification = intent.getParcelableExtra(NOTIFICATION);
        int notificationId = intent.getIntExtra(NOTIFICATION_ID, 0);
        notificationManager.notify(notificationId, notification);
    }

    public static void scheduleNotification(Context context, int id, String title, String msg, long delay, boolean repeat, int priority) {
        NotificationManager manager = (NotificationManager) context
                .getSystemService(Context.NOTIFICATION_SERVICE);

        Intent intent = SDLActivity.getContext().getPackageManager().getLaunchIntentForPackage(SDLActivity.getContext().getPackageName());
        intent.putExtra(PALHelper.LOCAL_NOTIFICATION_INTENT, String.valueOf(id));

        PendingIntent pendingIntent = PendingIntent.getActivity(
                context,
                0,
                intent,
                PendingIntent.FLAG_UPDATE_CURRENT);

        NotificationCompat.Builder _notification = new NotificationCompat
                .Builder(context, NOTIFICATION_CHANNEL)
                .setPriority(NotificationCompat.PRIORITY_HIGH)
                .setSmallIcon(SDLActivity.getContext().getResources().getIdentifier("ic_notification", "drawable", SDLActivity.getContext().getPackageName()))
                .setContentTitle(title)
                .setContentText(msg)
                .setDefaults(DEFAULT_ALL)
                .setContentIntent(pendingIntent)
                .setPriority(priority)
                .setAutoCancel(true);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            _notification.setChannelId(NOTIFICATION_CHANNEL);

            Uri ringToneUri = RingtoneManager.getDefaultUri(RingtoneManager.TYPE_NOTIFICATION);

            AudioAttributes attributes = new AudioAttributes.Builder()
                    .setUsage(AudioAttributes.USAGE_NOTIFICATION_RINGTONE)
                    .setContentType(AudioAttributes.CONTENT_TYPE_SONIFICATION)
                    .build();

            long[] pattern = new long[]{100, 200, 300, 400, 500, 400, 300, 200, 400};

            NotificationChannel notificationChannel = new NotificationChannel(
                    NOTIFICATION_CHANNEL, NOTIFICATION_NAME, NotificationManager.IMPORTANCE_HIGH);
            notificationChannel.enableVibration(true);
            notificationChannel.setVibrationPattern(pattern);
            notificationChannel.setSound(ringToneUri, attributes);
            manager.createNotificationChannel(notificationChannel);
        }

        Notification notification = _notification.build();

        Intent notificationIntent = new Intent(context, IGENotificationPublisher.class);
        notificationIntent.putExtra(IGENotificationPublisher.NOTIFICATION_ID, id);
        notificationIntent.putExtra(IGENotificationPublisher.NOTIFICATION, notification);
        PendingIntent _pendingIntent = PendingIntent.getBroadcast(context, id, notificationIntent, PendingIntent.FLAG_CANCEL_CURRENT);

        long futureInMillis = SystemClock.elapsedRealtime() + delay;
        AlarmManager alarmManager = (AlarmManager) context.getSystemService(Context.ALARM_SERVICE);
        alarmManager.set(AlarmManager.ELAPSED_REALTIME_WAKEUP, futureInMillis, _pendingIntent);
    }
}