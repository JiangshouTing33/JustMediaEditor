package just.android.justmediaeditor.widght

import android.app.Dialog
import android.content.Context
import android.view.LayoutInflater
import android.widget.Button
import android.widget.EditText
import android.widget.Spinner
import android.widget.Toast
import just.android.justmediaeditor.R
import just.android.justmediaeditor.getTextString
import just.android.justmediaeditor.param.FontName
import just.android.justmediaeditor.util.CommonUtil
import just.android.mediaeditor.param.FontTextOption
import java.io.File


class FontTextAddWidget(context: Context): Dialog(context) {
    interface FontTextAddListener {
        fun onResult(fontTextOption: FontTextOption)
    }

    fun initView(fontTextAddListener: FontTextAddListener?) {
        val view = LayoutInflater.from(context).inflate(R.layout.layout_add_font_text_widget, null)

        val etInputText = view.findViewById<EditText>(R.id.et_input_text)
        val etFontTextX = view.findViewById<EditText>(R.id.et_font_text_x)
        val etFontTextY = view.findViewById<EditText>(R.id.et_font_text_y)
        val etFontTextStart = view.findViewById<EditText>(R.id.et_font_text_start)
        val etFontTextEnd = view.findViewById<EditText>(R.id.et_font_text_end)
        val etFontColorR = view.findViewById<EditText>(R.id.et_font_color_r)
        val etFontColorG = view.findViewById<EditText>(R.id.et_font_color_g)
        val etFontColorB = view.findViewById<EditText>(R.id.et_font_color_b)
        val spiFont = view.findViewById<Spinner>(R.id.spi_font)

        view.findViewById<Button>(R.id.btn_add).setOnClickListener {
            if (etInputText.getTextString().isNullOrEmpty()) {
                Toast.makeText(context, "请输入文字", Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }

            if (etFontTextStart.getTextString().isNullOrEmpty()
                || etFontTextEnd.getTextString().isNullOrEmpty()) {
                Toast.makeText(context, "开始/结束时间不能为空", Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }

            fontTextAddListener?.onResult(
                FontTextOption(
                    etInputText.getTextString()!!,
                    getFontPath(spiFont.selectedItem as String),
                    etFontTextX.getTextString()?.toFloat() ?: 0F,
                    etFontTextY.getTextString()?.toFloat() ?: 0F,
                    etFontTextStart.getTextString()!!.toFloat(),
                    etFontTextEnd.getTextString()!!.toFloat(),
                    etFontColorR.getTextString()?.toFloat() ?: 0F,
                    etFontColorG.getTextString()?.toFloat() ?: 0F,
                    etFontColorB.getTextString()?.toFloat() ?: 0F
                )
            )
            dismiss()
        }
        setContentView(view)
    }

    override fun dismiss() {
        super.dismiss()
        CommonUtil.forceHideSoftInputWindow(context, currentFocus)
    }

    private fun getFontPath(fontNameString: String): String {
        val fontName = FontName.fromName(fontNameString)

        return if (fontName == null) {
            CommonUtil.getFontDirPath(context) + File.separator + FontName.FONT_JIANTI.path
        } else {
            CommonUtil.getFontDirPath(context) + File.separator + fontName.path
        }
    }
}