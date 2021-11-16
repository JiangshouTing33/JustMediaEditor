package just.android.justmediaeditor.constant

import android.os.Environment
import java.io.File

object JustMediaEditorConstant {
    const val EDITOR_TYPE = "editorType"

    const val EDITOR_TYPE_SINGLE_VIDEO = 0
    const val EDITOR_TYPE_MULTI_IMAGE = 1

    const val CROP_IMAGE_DIR = "cropImage"

    const val SHARED_PREFERENCE_NAME = "MEditor"

    const val SHARED_PREFERENCE_KEY_COPY_FONT_RESULT = "copyFontResult"

    const val SDK_NAME = "mediaLearn"
    const val MATERIAL_DIR_NAME = "material"
    const val FONT_DIR_NAME = "font"
    private const val EDITOR_DIR_NAME = "editor"

    var EDITOR_DST_PATH = Environment.getExternalStorageDirectory().absolutePath + File.separator + SDK_NAME + File.separator + EDITOR_DIR_NAME
}