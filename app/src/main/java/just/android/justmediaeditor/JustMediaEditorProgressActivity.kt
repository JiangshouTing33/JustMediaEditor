package just.android.justmediaeditor

import android.content.Context
import android.content.Intent
import android.os.Bundle
import android.widget.Button
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import just.android.justmediaeditor.util.CommonUtil
import just.android.mediaeditor.editor.JustMediaEditor
import just.android.mediaeditor.editor.JustPic2VideoEditor
import just.android.mediaeditor.editor.MediaEditorMessageType
import just.android.justmediaeditor.util.JustMEditorLogUtil
import just.android.justmediaeditor.widght.EditorSquareProcessView
import just.android.justmediaeditor.constant.JustMediaEditorConstant
import just.android.mediaeditor.editor.IEditEventListener

class JustMediaEditorProgressActivity: AppCompatActivity() {
    private lateinit var editorProgress: EditorSquareProcessView
    private lateinit var btnCheck: Button

    private var maxProgress = 100F

    companion object {
        fun getIntent(context: Context, editorType: Int): Intent {
            val intent = Intent(context, JustMediaEditorProgressActivity::class.java)
            intent.putExtra(JustMediaEditorConstant.EDITOR_TYPE, editorType)
            return intent
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        setContentView(R.layout.activity_editor_progress)
        initView()
        registerEditorListener()
    }

    override fun onDestroy() {
        super.onDestroy()
        JustMediaEditor.stopEditor()
        JustPic2VideoEditor.stopEditor()
    }

    private fun initView() {
        editorProgress = findViewById(R.id.prg_editor)
        btnCheck = findViewById(R.id.btn_check)

        btnCheck.setOnClickListener {
            val intent = Intent(Intent.ACTION_VIEW)
            intent.type = "vnd.android.cursor.dir/video"
            startActivity(intent)
        }
    }

    private fun registerVideoEditorEventListener() {
        JustMediaEditor.registerEditEventListener(object : IEditEventListener {
            override fun onEditEvent(
                messageType: MediaEditorMessageType?,
                messageValue: Float
            ) {
                JustMEditorLogUtil.d("messageType = ${messageType?.value ?: -1}, messageValue = $messageValue")
                runOnUiThread {
                    when (messageType) {
                        MediaEditorMessageType.TYPE_DURATION -> {
                            maxProgress = if (messageValue == 0F) 100F else messageValue
                        }
                        MediaEditorMessageType.TYPE_CURRENT_TIME -> editorProgress.setCurrentPogress(
                            messageValue * 100 / maxProgress
                        )
                        MediaEditorMessageType.TYPE_DONE -> {
                            Toast.makeText(
                                this@JustMediaEditorProgressActivity,
                                "视频编辑成功",
                                Toast.LENGTH_SHORT
                            ).show()
                            editorProgress.setCurrentPogress(100F)
                            btnCheck.isEnabled = true
                            JustMediaEditor.dstPath?.let {
                                CommonUtil.sendScanFileBroadcast(
                                    this@JustMediaEditorProgressActivity,
                                    it
                                )
                            }
                        }
                    }
                }
            }
        })
    }

    private fun registerPic2VideoEditorEventListener() {
        JustPic2VideoEditor.registerEditorEventListener(object : IEditEventListener {
            override fun onEditEvent(
                messageType: MediaEditorMessageType?,
                messageValue: Float
            ) {
                JustMEditorLogUtil.d("messageType = ${messageType?.value ?: -1}, messageValue = $messageValue")
                runOnUiThread {
                    when (messageType) {
                        MediaEditorMessageType.TYPE_DURATION -> {
                            maxProgress = if (messageValue == 0F) 100F else messageValue
                        }
                        MediaEditorMessageType.TYPE_CURRENT_TIME -> editorProgress.setCurrentPogress(
                            messageValue * 100 / maxProgress
                        )
                        MediaEditorMessageType.TYPE_DONE -> {
                            Toast.makeText(
                                this@JustMediaEditorProgressActivity,
                                "图片合成成功",
                                Toast.LENGTH_SHORT
                            ).show()
                            editorProgress.setCurrentPogress(100F)
                            btnCheck.isEnabled = true
                            JustPic2VideoEditor.dstPath?.let {
                                CommonUtil.sendScanFileBroadcast(
                                    this@JustMediaEditorProgressActivity,
                                    it
                                )
                            }
                        }
                    }
                }
            }
        })
    }

    private fun registerEditorListener() {
        when(intent.getIntExtra(JustMediaEditorConstant.EDITOR_TYPE, -1)) {
            JustMediaEditorConstant.EDITOR_TYPE_SINGLE_VIDEO -> registerVideoEditorEventListener()
            JustMediaEditorConstant.EDITOR_TYPE_MULTI_IMAGE -> registerPic2VideoEditorEventListener()
        }
    }
}