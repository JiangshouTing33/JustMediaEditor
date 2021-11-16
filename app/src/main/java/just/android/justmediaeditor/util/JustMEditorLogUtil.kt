package just.android.justmediaeditor.util

object JustMEditorLogUtil {
    private const val TAG = "JustMEditorLog"

    fun d(msg: String) {
        android.util.Log.d(TAG, msg)
    }
    fun e(msg: String) {
        android.util.Log.e(TAG, msg)
    }
    fun u(msg: String) {
        android.util.Log.i(TAG, msg)
    }
}