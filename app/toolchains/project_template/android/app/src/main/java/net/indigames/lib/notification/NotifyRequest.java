package net.indigames.lib.notification;

import androidx.work.Data;
import androidx.work.OneTimeWorkRequest;
import androidx.work.PeriodicWorkRequest;

import androidx.work.WorkManager;
import androidx.work.WorkRequest;

import org.libsdl.app.SDLActivity;

import java.util.concurrent.TimeUnit;

public class NotifyRequest {
    public static final String workTag = "NotificationWork";

    public static void enqueueEvent(NotifyEvent event) {
//        enqueueEvent(event.mID, event.mTitle, event.mMessage, event.mDelay, event.mRepeatable, event.mPriority);
        IGENotificationPublisher.scheduleNotification(SDLActivity.getContext(), event.mID, event.mTitle, event.mMessage, event.mDelay, event.mRepeatable, event.mPriority);
    }

    public static void workerEnqueueEvent(int id, String title, String message, long delay, boolean repeat, int priority) {
        Data inputData = new Data.Builder()
                .putInt(NotifyEvent.EventIDTag, id)
                .putString(NotifyEvent.EventTitleTag, title)
                .putString(NotifyEvent.EventMessageTag, message)
                .putInt(NotifyEvent.EventPriorityTag, priority)
                .build();

        WorkRequest request;
        if(!repeat) {
            request = new OneTimeWorkRequest.Builder(NotifyWorker.class)
                    .setInitialDelay(delay, TimeUnit.MILLISECONDS)
                    .setInputData(inputData)
                    .addTag(workTag)
                    .build();
        } else {
            delay = Math.max(delay, PeriodicWorkRequest.MIN_PERIODIC_INTERVAL_MILLIS);
            request = new PeriodicWorkRequest.Builder(NotifyWorker.class, delay, TimeUnit.MILLISECONDS,
                                                        PeriodicWorkRequest.MIN_PERIODIC_FLEX_MILLIS, TimeUnit.MILLISECONDS)
                    .setInputData(inputData)
                    .addTag(workTag)
                    .build();
        }
        WorkManager.getInstance(SDLActivity.getContext()).enqueue(request);
    }

    public static void enqueueEvent(int id, String title, String message, long delay, boolean repeat, int priority) {
        IGENotificationPublisher.scheduleNotification(SDLActivity.getContext(), id, title, message, delay, repeat, priority);
    }
}
