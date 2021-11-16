package just.android.mediaeditor.util

import android.content.Context
import android.graphics.*
import android.net.Uri
import java.lang.Exception

object JustMEditorCommonUtil {
    fun scaleImage(bm: Bitmap?, newWidth: Int, newHeight: Int): Bitmap? {
        if (bm == null) return null
        if (newWidth <= 0 || newHeight <= 0) return bm

        val width = bm.width
        val height = bm.height

        val scaleWidth = newWidth.toFloat() / width
        val scaleHeight = newHeight.toFloat() / height

        // 保持宽高比缩放，以长边为主
        val scaleRatio = scaleHeight.coerceAtMost(scaleWidth)

        val scaledImage = Bitmap.createBitmap(newWidth, newHeight, Bitmap.Config.ARGB_8888)
        val canvas = Canvas(scaledImage)

        val paint = Paint()
        paint.color = Color.BLACK
        paint.style = Paint.Style.FILL
        canvas.drawRect(0f, 0f, newWidth.toFloat(), newHeight.toFloat(), paint)
        canvas.drawFilter =
            PaintFlagsDrawFilter(0, Paint.ANTI_ALIAS_FLAG or Paint.FILTER_BITMAP_FLAG)
        val matrix = Matrix()
        matrix.postScale(scaleRatio, scaleRatio)
        if (scaleWidth > scaleHeight) {
            matrix.postTranslate((newWidth - width * scaleRatio) / 2f, 0f)
        } else {
            matrix.postTranslate(0f, (newHeight - height * scaleRatio) / 2f)
        }
        canvas.drawBitmap(bm, matrix, null)
        if (!bm.isRecycled) {
            bm.recycle()
        }
        return scaledImage
    }

    fun getBitmap(context: Context, resId: Int) : Bitmap {
        val option = BitmapFactory.Options()
        option.inScaled = false
        return BitmapFactory.decodeResource(context.resources, resId, option)
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
}