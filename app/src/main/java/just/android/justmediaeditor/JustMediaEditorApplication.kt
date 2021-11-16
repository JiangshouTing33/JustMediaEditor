package just.android.justmediaeditor

import android.app.Application
import just.android.justmediaeditor.util.CommonUtil

class JustMediaEditorApplication: Application() {

    override fun onCreate() {
        super.onCreate()

        CommonUtil.copyAssetsToDst(this, "fonts")
    }
}