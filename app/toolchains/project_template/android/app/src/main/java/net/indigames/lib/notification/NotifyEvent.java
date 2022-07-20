package net.indigames.lib.notification;

public class NotifyEvent {
    public static final String EventIDTag = "IGE_EVENT_ID";
    public static final String EventTitleTag = "IGE_EVENT_TITLE";
    public static final String EventMessageTag = "IGE_EVENT_MESSAGE";
    public static final String EventPriorityTag = "IGE_EVENT_PRIORITY";

    public int mID;
    public String mTitle;
    public String mMessage;
    public long mDelay;
    public boolean mRepeatable;
    public int mPriority;

    NotifyEvent(int id, String title, String msg, int delay, boolean repeat, int priority) {
        mID = id;
        mTitle = title;
        mMessage = msg;
        mDelay = delay;
        mRepeatable = repeat;
        mPriority = priority;
    }
}
