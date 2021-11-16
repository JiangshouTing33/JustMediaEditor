package just.android.justmediaeditor.util

import android.content.Context
import android.content.Intent
import android.content.SharedPreferences
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.net.Uri
import android.os.Environment
import android.view.View
import android.view.inputmethod.InputMethodManager
import just.android.justmediaeditor.param.LutFilter
import just.android.justmediaeditor.param.TransitionType
import just.android.justmediaeditor.constant.JustMediaEditorConstant
import java.io.*
import java.lang.Exception
import java.text.SimpleDateFormat
import java.util.*

object CommonUtil {
    fun generateCropImagePath(context: Context): String {
        val coverImageDir = File(context.externalCacheDir?.absolutePath + File.separator + JustMediaEditorConstant.CROP_IMAGE_DIR)
        if (!coverImageDir.exists()) {
            coverImageDir.mkdirs()
        }

        return coverImageDir.absolutePath + File.separator + getRandomString(10) + System.currentTimeMillis() + ".jpg"
    }

    fun generateBitmapFromUriString(context: Context, uriString: String?): Bitmap? {
        var bitmap: Bitmap? = null
        try {
            val inputStream = context.contentResolver.openInputStream(Uri.parse(uriString))
            bitmap = BitmapFactory.decodeStream(inputStream)
        } catch (e: Exception) {
            e.printStackTrace()
        }

        return bitmap
    }

    fun generateDstUrl(): String {
        val dstDir = File(JustMediaEditorConstant.EDITOR_DST_PATH)

        if (!dstDir.exists()) {
            dstDir.mkdirs()
        }

        return JustMediaEditorConstant.EDITOR_DST_PATH + File.separator + getRandomString(10) + System.currentTimeMillis() + ".mp4"
    }

    fun getTransitionFShaderStr(context: Context, value: Int?): String? {
        val transitionType = TransitionType.fromInt(value)

        return if (transitionType != null) {
            readStringFromAssets(context, transitionType.fileName)
        } else {
            null
        }
    }

    @JvmStatic
    fun dp2px(context: Context, dp: Float): Int{
        if (dp == 0f) {
            return 0
        }
        return (dp * context.resources.displayMetrics.density + 0.5f).toInt()
    }

    fun forceHideSoftInputWindow(context: Context, view: View?) {
        val imm: InputMethodManager? =
            context.getSystemService(Context.INPUT_METHOD_SERVICE) as InputMethodManager?
        imm?.hideSoftInputFromWindow(view?.windowToken, InputMethodManager.HIDE_NOT_ALWAYS)
    }

    fun getLutBitmap(context: Context, value: Int?): Bitmap {
        val lutFilter = LutFilter.fromInt(value)

        val drawableId = lutFilter?.lutDrawableId ?: LutFilter.AIQING.lutDrawableId

        return getBitmap(context, drawableId)
    }

    fun getBitmap(context: Context, resId: Int) : Bitmap {
        val option = BitmapFactory.Options()
        option.inScaled = false
        return BitmapFactory.decodeResource(context.resources, resId, option)
    }

    fun sendScanFileBroadcast(context: Context, path: String) {
        val i = Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE)
        i.data = Uri.fromFile(File(path))
        context.sendBroadcast(i)
    }

    fun copyAssetsToDst(context: Context, srcPath: String) {
        if (checkCopyFontResult(context)) return

        var inputStream: InputStream? = null
        var outputStream: FileOutputStream? = null
        val dstPath = getFontDirPath(context)
        try {
            val dir = File(dstPath)
            if (!dir.exists()) dir.mkdirs()
            val fileNames = context.assets.list(srcPath)
            fileNames?.forEach {
                val file = File(dstPath + File.separator + it)
                if (!file.exists()) file.createNewFile()
                inputStream = context.assets.open(srcPath + File.separator + it)
                outputStream = FileOutputStream(file)
                val bufferArray = ByteArray(1024)
                var byteCount = 0
                while (inputStream?.read(bufferArray)?.apply { byteCount = this}  != -1) {
                    outputStream?.write(bufferArray, 0, byteCount)
                }
                outputStream?.flush()
                inputStream?.close()
                outputStream?.close()
            }
            saveCopyFontResult(context, true)
        } catch (e: Exception) {
            e.printStackTrace()
        } finally {
            outputStream?.flush()
            inputStream?.close()
            outputStream?.close()
        }
    }

    fun getFontDirPath(context: Context):String {
        return StringBuilder()
            .append(context.applicationContext.filesDir.absolutePath)
            .append(File.separator)
            .append(JustMediaEditorConstant.SDK_NAME)
            .append(File.separator)
            .append(JustMediaEditorConstant.MATERIAL_DIR_NAME)
            .append(File.separator)
            .append(JustMediaEditorConstant.FONT_DIR_NAME)
            .toString()
    }

    fun saveImage(bmp: Bitmap): Int {
        //生成路径
        val root: String = Environment.getExternalStorageDirectory().absolutePath
        val dirName = "mediaLearn"
        val appDir = File(root, dirName)
        if (!appDir.exists()) {
            appDir.mkdirs()
        }

        //文件名为时间
        val timeStamp = System.currentTimeMillis()
        val sdf = SimpleDateFormat("yyyy-MM-dd HH:mm:ss")
        val sd: String = sdf.format(Date(timeStamp))
        val fileName = "$sd.jpg"

        //获取文件
        val file = File(appDir, fileName)
        var fos: FileOutputStream? = null
        try {
            fos = FileOutputStream(file)
            bmp.compress(Bitmap.CompressFormat.JPEG, 100, fos)
            fos.flush()
            return 2
        } catch (e: FileNotFoundException) {
            e.printStackTrace()
        } catch (e: IOException) {
            e.printStackTrace()
        } finally {
            try {
                fos?.close()
            } catch (e: IOException) {
                e.printStackTrace()
            }
        }
        return -1
    }

    private fun saveCopyFontResult(context: Context, value: Boolean) {
        getMediaEditorSharedPreference(context).edit().putBoolean(JustMediaEditorConstant.SHARED_PREFERENCE_KEY_COPY_FONT_RESULT, value).apply()
    }

    private fun checkCopyFontResult(context: Context): Boolean
            = getMediaEditorSharedPreference(context).getBoolean(JustMediaEditorConstant.SHARED_PREFERENCE_KEY_COPY_FONT_RESULT, false)

    private fun getMediaEditorSharedPreference(context: Context): SharedPreferences
            = context.getSharedPreferences(JustMediaEditorConstant.SHARED_PREFERENCE_NAME,
        Context.MODE_PRIVATE
    )

    private fun getRandomString(length: Int): String {
        val base = "abcdefghijklmnopqrstuvwxyz0123456789"
        val random = Random()
        val sb = StringBuffer()
        for (i in 0 until length) {
            val number = random.nextInt(base.length)
            sb.append(base[number])
        }
        return sb.toString()
    }

    private fun readStringFromAssets(context: Context, fileName: String): String {
        val stringBuilder = StringBuilder()
        try {
            val bufferedReader = BufferedReader(
                InputStreamReader(
                    context.assets.open(fileName)
                )
            )
            var line: String?
            while (bufferedReader.readLine().also { line = it } != null) {
                stringBuilder.append(line).append("\n")
            }
        } catch (e: IOException) {
            e.printStackTrace()
        }
        return stringBuilder.toString()
    }
}