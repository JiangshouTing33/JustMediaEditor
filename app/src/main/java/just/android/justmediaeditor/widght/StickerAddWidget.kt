package just.android.justmediaeditor.widght

import android.app.Dialog
import android.content.Context
import android.graphics.Bitmap
import android.view.LayoutInflater
import android.widget.Button
import android.widget.EditText
import android.widget.ImageView
import android.widget.Toast
import just.android.justmediaeditor.R
import just.android.justmediaeditor.getTextString
import just.android.justmediaeditor.util.CommonUtil
import just.android.mediaeditor.param.StickerImageOption

class StickerAddWidget(context: Context): Dialog(context) {
    private lateinit var imStickerImage: ImageView
    private var stickerImage: Bitmap? = null

    interface StickerAddListener {
        fun onPictureSelect()
        fun onResult(stickerImageOption: StickerImageOption)
    }

    fun initView(stickerAddListener: StickerAddListener?) {
        val view = LayoutInflater.from(context).inflate(R.layout.layout_add_sticker_widget, null)

        imStickerImage = view.findViewById(R.id.im_sticker_image)
        val etStickerX: EditText = view.findViewById(R.id.et_sticker_image_x)
        val etStickerY: EditText = view.findViewById(R.id.et_sticker_image_y)
        val etStickerStart: EditText = view.findViewById(R.id.et_sticker_image_start)
        val etStickerEnd: EditText = view.findViewById(R.id.et_sticker_image_end)

        view.findViewById<Button>(R.id.btn_select_sticker_image).setOnClickListener {
            stickerAddListener?.onPictureSelect()
        }

        view.findViewById<Button>(R.id.btn_add).setOnClickListener {
            if (stickerImage == null) {
                Toast.makeText(context, "请选择图片", Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }
            if (etStickerStart.getTextString().isNullOrEmpty() || etStickerEnd.getTextString().isNullOrEmpty()) {
                Toast.makeText(context, "开始/结束时间不能为空", Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }

            stickerAddListener?.onResult(
                StickerImageOption(
                    stickerImage!!,
                    etStickerX.getTextString()?.toFloat() ?: 1F,
                    etStickerY.getTextString()?.toFloat() ?: 1F,
                    etStickerStart.getTextString()!!.toFloat(),
                    etStickerEnd.getTextString()!!.toFloat()))
            dismiss()
        }

        setContentView(view)
    }

    fun setStickerImageFromAlbum(bitmap: Bitmap) {
        this.stickerImage = bitmap
        imStickerImage.setImageBitmap(bitmap)
    }

    override fun dismiss() {
        super.dismiss()
        CommonUtil.forceHideSoftInputWindow(context, currentFocus)
    }
}