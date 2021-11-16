package just.android.justmediaeditor

import android.content.Intent
import android.content.pm.PackageManager
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.net.Uri
import android.os.Bundle
import android.provider.MediaStore
import android.view.View
import android.widget.*
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import just.android.justmediaeditor.adapter.FontTextListAdapter
import just.android.justmediaeditor.adapter.StickerListAdapter
import just.android.justmediaeditor.param.FrameRateOption
import just.android.justmediaeditor.param.LutFilter
import just.android.justmediaeditor.param.ResolutionOption
import just.android.justmediaeditor.param.TransitionType
import just.android.justmediaeditor.util.CommonUtil
import just.android.mediaeditor.param.*
import just.android.mediaeditor.editor.JustMediaEditor
import just.android.mediaeditor.editor.JustPic2VideoEditor
import just.android.justmediaeditor.widght.FontTextAddWidget
import just.android.justmediaeditor.widght.StickerAddWidget
import just.android.justmediaeditor.constant.JustMediaEditorConstant
import java.io.File

fun TextView.getTextString(): String? = if (this.text.isNullOrEmpty()) null else this.text.toString()

class JustMediaEditorDemoActivity: AppCompatActivity(), View.OnClickListener{
    companion object {
        const val EXTERNAL_STORAGE_REQUEST_CODE = 0x11
        const val SELECT_VIDEO_REQUEST_CODE = 0x12
        const val SELECT_AUDIO_REQUEST_CODE = 0x13
        const val SELECT_COVER_IMAGE_REQUEST_CODE = 0x14
        const val SELECT_MARK_IMAGE_REQUEST_CODE = 0x15
        const val CROP_COVER_IMAGE_REQUEST_CODE = 0x16
        const val CROP_MARK_IMAGE_REQUEST_CODE = 0x17
        const val SELECT_BACKGROUND_MUSIC_REQUEST_CODE = 0x18
        const val SELECT_STICKER_IMAGE_REQUEST_CODE = 0x19
        const val CROP_STICKER_IMAGE_REQUEST_CODE = 0x20
        const val SELECT_MULTI_IMAGE_REQUEST_CODE = 0x21
    }

    private lateinit var tvVideoPath: TextView
    private lateinit var tvAudioPath: TextView
    private lateinit var tvBackgroundPath: TextView
    private lateinit var imCoverImage: ImageView
    private lateinit var imMarkImage: ImageView
    private lateinit var spinnerFilter: Spinner
    private lateinit var spinnerSpeed: Spinner
    private lateinit var cbWithAudio: CheckBox
    private lateinit var etMarkImageX: EditText
    private lateinit var etMarkImageY: EditText
    private lateinit var rcySticker: RecyclerView
    private lateinit var rcyText: RecyclerView
    private lateinit var tvPicCount: TextView
    private lateinit var spinnerTransitionType: Spinner
    private lateinit var llSelectVideo: LinearLayout
    private lateinit var llSelectMultiPic: LinearLayout
    private lateinit var llChoseTransitionType: LinearLayout
    private lateinit var llSelectBackgroundMusic: LinearLayout
    private lateinit var llChoseFilter: LinearLayout
    private lateinit var llChoseSpeed: LinearLayout
    private lateinit var spinnerResolutionVideo: Spinner
    private lateinit var spinnerResolutionImage: Spinner
    private lateinit var spinnerFrameRate: Spinner
    private lateinit var tvClearCoverImage: TextView
    private lateinit var tvClearMarkImage: TextView
    private lateinit var tvClearBackgroundMusic: TextView
    private lateinit var tvSelectFrameRate: TextView

    private var coverImageBitmap: Bitmap? = null
    private var markImageBitmap: Bitmap? = null

    private var stickerAddWidget: StickerAddWidget? = null

    private val stickerOptionList: MutableList<StickerImageOption> = arrayListOf()
    private var stickerListAdapter: StickerListAdapter? = null

    private val textOptionList: MutableList<FontTextOption> = arrayListOf()
    private var fontTextListAdapter: FontTextListAdapter? = null

    private val picturePathList: MutableList<String> = arrayListOf()

    private var currentEditorType = JustMediaEditorConstant.EDITOR_TYPE_SINGLE_VIDEO

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_media_editor_demo)

        checkPermission()
        initView()
    }

    private fun initView() {
        tvVideoPath = findViewById(R.id.tv_video_path)
        tvAudioPath = findViewById(R.id.tv_audio_path)
        imCoverImage = findViewById(R.id.im_cover_image)
        imMarkImage = findViewById(R.id.im_mark_image)
        spinnerFilter = findViewById(R.id.spi_filter)
        spinnerSpeed = findViewById(R.id.spi_speed)
        cbWithAudio = findViewById(R.id.cb_with_audio)
        tvBackgroundPath = findViewById(R.id.tv_background_music_path)
        etMarkImageX = findViewById(R.id.et_mark_image_x)
        etMarkImageY = findViewById(R.id.et_mark_image_y)
        rcySticker = findViewById(R.id.rcy_sticker)
        rcyText = findViewById(R.id.rcy_text)
        tvPicCount = findViewById(R.id.tv_pic_count)
        spinnerTransitionType = findViewById(R.id.spi_transition_type)
        llSelectVideo = findViewById(R.id.ll_select_video)
        llSelectMultiPic = findViewById(R.id.ll_select_multi_pic)
        llChoseTransitionType = findViewById(R.id.ll_chose_transition_type)
        llSelectBackgroundMusic = findViewById(R.id.ll_select_background_music)
        llChoseFilter = findViewById(R.id.ll_chose_filter)
        llChoseSpeed = findViewById(R.id.ll_chose_speed)
        spinnerResolutionVideo = findViewById(R.id.spi_resolution_video)
        spinnerResolutionImage = findViewById(R.id.spi_resolution_image)
        spinnerFrameRate = findViewById(R.id.spi_frame_rate)
        tvClearCoverImage = findViewById(R.id.tv_clear_cover_image)
        tvClearMarkImage = findViewById(R.id.tv_clear_mark_image)
        tvClearBackgroundMusic = findViewById(R.id.tv_clear_background_music_path)
        tvSelectFrameRate = findViewById(R.id.tv_select_frame_rate)

        spinnerSpeed.setSelection(1)
        spinnerResolutionImage.setSelection(2)
        spinnerFrameRate.setSelection(2)

        findViewById<Button>(R.id.btn_select_video).setOnClickListener(this)
        findViewById<Button>(R.id.btn_select_audio).setOnClickListener(this)
        findViewById<Button>(R.id.btn_select_background_music).setOnClickListener(this)
        findViewById<Button>(R.id.btn_select_cover_image).setOnClickListener(this)
        findViewById<Button>(R.id.btn_select_mark_image).setOnClickListener(this)
        findViewById<Button>(R.id.btn_add_sticker_image).setOnClickListener(this)
        findViewById<Button>(R.id.btn_add_text).setOnClickListener(this)
        findViewById<Button>(R.id.btn_start_editor).setOnClickListener(this)
        findViewById<Button>(R.id.btn_select_multi_pic).setOnClickListener(this)
        tvClearCoverImage.setOnClickListener(this)
        tvClearMarkImage.setOnClickListener(this)
        tvClearBackgroundMusic.setOnClickListener(this)

        findViewById<RadioGroup>(R.id.edit_type_group).setOnCheckedChangeListener { _, checkedId ->
            if (checkedId == R.id.rb_video_editor) {
                currentEditorType = JustMediaEditorConstant.EDITOR_TYPE_SINGLE_VIDEO
            } else if (checkedId == R.id.rb_picture_editor) {
                currentEditorType = JustMediaEditorConstant.EDITOR_TYPE_MULTI_IMAGE
            }
            updateEditorParamItemVisible()
        }

        initStickerRecyclerView()
        initFontTextRecyclerView()
    }

    override fun onClick(v: View?) {
        v?.let {
            when (it.id) {
                R.id.btn_select_video -> {
                    val i = Intent(Intent.ACTION_PICK)
                    i.type = "video/*"
                    startActivityForResult(
                        Intent.createChooser(i, "选择Video轨道"),
                        SELECT_VIDEO_REQUEST_CODE
                    )
                }

                R.id.btn_select_audio -> {
                    val i = Intent(Intent.ACTION_GET_CONTENT)
                    i.type = "audio/mpeg"
                    startActivityForResult(
                        Intent.createChooser(i, "选择Audio轨道"),
                        SELECT_AUDIO_REQUEST_CODE
                    )
                }

                R.id.btn_select_cover_image -> {
                    val i = Intent(Intent.ACTION_PICK)
                    i.type = "image/*"
                    startActivityForResult(
                        Intent.createChooser(i, "选择封面图"),
                        SELECT_COVER_IMAGE_REQUEST_CODE
                    )
                }

                R.id.btn_select_mark_image -> {
                    val i = Intent(Intent.ACTION_PICK)
                    i.type = "image/*"
                    startActivityForResult(
                        Intent.createChooser(i, "选择水印图片"),
                        SELECT_MARK_IMAGE_REQUEST_CODE
                    )
                }
                R.id.btn_start_editor -> startVideoEditor()
                R.id.btn_select_background_music -> {
                    val i = Intent(Intent.ACTION_GET_CONTENT)
                    i.type = "audio/mpeg"
                    startActivityForResult(
                        Intent.createChooser(i, "选择背景音乐"),
                        SELECT_BACKGROUND_MUSIC_REQUEST_CODE
                    )
                }
                R.id.btn_add_sticker_image -> {
                    stickerAddWidget = StickerAddWidget(this@JustMediaEditorDemoActivity)
                    stickerAddWidget?.initView(object : StickerAddWidget.StickerAddListener {
                        override fun onPictureSelect() {
                            val i = Intent(Intent.ACTION_PICK)
                            i.type = "image/*"
                            startActivityForResult(
                                Intent.createChooser(i, "选择贴图"),
                                SELECT_STICKER_IMAGE_REQUEST_CODE
                            )
                        }

                        override fun onResult(stickerImageOption: StickerImageOption) {
                            stickerOptionList.add(stickerImageOption)
                            stickerListAdapter?.notifyDataSetChanged()
                        }

                    })
                    stickerAddWidget?.show()
                }
                R.id.btn_add_text -> {
                    val fontTextAddWidget = FontTextAddWidget(this@JustMediaEditorDemoActivity)
                    fontTextAddWidget.initView(object : FontTextAddWidget.FontTextAddListener {
                        override fun onResult(fontTextOption: FontTextOption) {
                            textOptionList.add(fontTextOption)
                            fontTextListAdapter?.notifyDataSetChanged()
                        }
                    })
                    fontTextAddWidget.show()
                }
                R.id.btn_select_multi_pic -> {
                    val i = Intent()
                    i.type = "image/*"
                    i.action = Intent.ACTION_GET_CONTENT
                    i.putExtra(Intent.EXTRA_ALLOW_MULTIPLE, true)
                    startActivityForResult(
                        Intent.createChooser(i, "选择图片素材"),
                        SELECT_MULTI_IMAGE_REQUEST_CODE
                    )
                }
                R.id.tv_clear_cover_image -> {
                    coverImageBitmap = null
                    imCoverImage.setImageBitmap(null)
                    tvClearCoverImage.visibility = View.GONE
                }
                R.id.tv_clear_mark_image -> {
                    markImageBitmap = null
                    imMarkImage.setImageBitmap(null)
                    tvClearMarkImage.visibility = View.GONE
                }
                R.id.tv_clear_background_music_path -> {
                    tvBackgroundPath.text = ""
                    tvClearBackgroundMusic.visibility = View.GONE
                }
                else -> return
            }
        }
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)
        if (resultCode == RESULT_OK) {
            when (requestCode) {
                SELECT_VIDEO_REQUEST_CODE -> {
                    val path = getPathFromSelectResult(data) ?: ""
                    tvVideoPath.text = path
                    tvAudioPath.text = path
                }
                SELECT_AUDIO_REQUEST_CODE -> tvAudioPath.text = getPathFromSelectResult(data) ?: ""
                SELECT_COVER_IMAGE_REQUEST_CODE -> {
                    val cropCoverImageFile = File(CommonUtil.generateCropImagePath(this@JustMediaEditorDemoActivity))
                    if (!cropCoverImageFile.exists()) {
                        cropCoverImageFile.createNewFile()
                    }
                    startActivityForResult(
                        generateGalleryCropIntent(data?.data, cropCoverImageFile),
                        CROP_COVER_IMAGE_REQUEST_CODE)
                }
                CROP_COVER_IMAGE_REQUEST_CODE -> {
                    val bitmap = CommonUtil.generateBitmapFromUriString(this@JustMediaEditorDemoActivity, data?.action)
                    if (bitmap != null) {
                        coverImageBitmap = bitmap
                        imCoverImage.setImageBitmap(bitmap)
                        tvClearCoverImage.visibility = View.VISIBLE
                    }
                }
                SELECT_MARK_IMAGE_REQUEST_CODE -> {
                    val bitmap = getBitmapFromSelectResult(data)
                    if (bitmap != null) {
                        markImageBitmap = bitmap
                        imMarkImage.setImageBitmap(bitmap)
                        tvClearMarkImage.visibility = View.VISIBLE
                    }
                }
                SELECT_STICKER_IMAGE_REQUEST_CODE -> {
                    val bitmap = getBitmapFromSelectResult(data)
                    if (bitmap != null) {
                        stickerAddWidget?.setStickerImageFromAlbum(bitmap)
                    }
                }
                SELECT_BACKGROUND_MUSIC_REQUEST_CODE ->  {
                    tvBackgroundPath.text = getPathFromSelectResult(data) ?: ""
                    tvClearBackgroundMusic.visibility = View.VISIBLE
                }
                SELECT_MULTI_IMAGE_REQUEST_CODE -> {
                    picturePathList.clear()
                    val images = data?.clipData
                    images?.let {
                        for (i in 0 until it.itemCount) {
                            picturePathList.add(images.getItemAt(i).uri.toString())
                        }
                    }
                    tvPicCount.text = String.format(getString(R.string.chosen_pic_count), picturePathList.size)
                }
            }
        }
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        when(requestCode) {
            EXTERNAL_STORAGE_REQUEST_CODE -> {
                if (grantResults[0] != PackageManager.PERMISSION_GRANTED) {
                    finish()
                }
            }
        }
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
    }

    private fun checkPermission() {
        if (ContextCompat.checkSelfPermission(
                this,
                android.Manifest.permission.WRITE_EXTERNAL_STORAGE
            ) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(
                this,
                arrayOf(
                    android.Manifest.permission.READ_EXTERNAL_STORAGE,
                    android.Manifest.permission.WRITE_EXTERNAL_STORAGE
                ), EXTERNAL_STORAGE_REQUEST_CODE
            )
        }
    }

    private fun getPathFromSelectResult(intent: Intent?): String? {
        var path: String? = null
        if (intent != null && intent.data != null) {
            val filePathColumn = arrayOf(MediaStore.Video.Media.DATA)
            val cursor = contentResolver.query(
                intent.data!!,
                filePathColumn, null, null, null
            )
            cursor!!.moveToFirst()
            val columnIndex = cursor.getColumnIndex(filePathColumn[0])
            path = cursor.getString(columnIndex)
            cursor.close()
        }

        return path
    }

    private fun getBitmapFromSelectResult(intent: Intent?): Bitmap? {
        val path = getPathFromSelectResult(intent)
        return if (path.isNullOrEmpty()) null else BitmapFactory.decodeFile(path)
    }

    private fun generateGalleryCropIntent(uri: Uri?, outputFile: File): Intent {
        val galleryCropIntent = Intent("com.android.camera.action.CROP")

        galleryCropIntent.setDataAndType(uri, "image/*")

        galleryCropIntent.putExtra("crop", true)
        galleryCropIntent.putExtra("scale", true)

        galleryCropIntent.putExtra("return-data", false)

        if (!outputFile.exists()) {
            outputFile.createNewFile()
        }

        val imageUri = Uri.fromFile(outputFile)

        if (imageUri != null){
            galleryCropIntent.putExtra(MediaStore.EXTRA_OUTPUT, imageUri)
            galleryCropIntent.putExtra("outputFormat", Bitmap.CompressFormat.JPEG.toString())
        }

        return galleryCropIntent
    }

    private fun startVideoEditor() {
        val resolutionOption = if (isVideoEditor()) ResolutionOption.fromName(spinnerResolutionVideo.selectedItem as String)
                else ResolutionOption.fromName(spinnerResolutionImage.selectedItem as String)

        if (currentEditorType == JustMediaEditorConstant.EDITOR_TYPE_SINGLE_VIDEO) {
            val vEditorParamBuilder = VideoEditorParam.Builder()
            vEditorParamBuilder
                .setVideoUrl(tvVideoPath.getTextString())
                .setBackgroundMusicUrl(tvBackgroundPath.getTextString())
                .setLutFilterBitmap(CommonUtil.getLutBitmap(this@JustMediaEditorDemoActivity,
                    LutFilter.fromName(spinnerFilter.selectedItem as String)?.lutValue))
                .setSpeedOption(SpeedOption.fromName(spinnerSpeed.selectedItem as String))
                .setSpeedWithAudio(cbWithAudio.isChecked)
                .setWidth(resolutionOption?.width)
                .setHeight(resolutionOption?.height)
                .setBitrate(resolutionOption?.bitRate)
                .setAudioUrl(tvAudioPath.getTextString())
                .setDstUrl(CommonUtil.generateDstUrl())
                .setCoverImage(coverImageBitmap)
                .setStickerImageOptionList(stickerOptionList)
                .setFontTextOptionList(textOptionList)
            if (markImageBitmap != null) {
                vEditorParamBuilder.setMarkImageOption(MarkImageOption(
                    markImageBitmap!!,
                    etMarkImageX.getTextString()?.toFloat() ?: 1F,
                    etMarkImageY.getTextString()?.toFloat() ?: 1F))
            }

            JustMediaEditor.startEditor(this@JustMediaEditorDemoActivity, vEditorParamBuilder.build())
        } else {
            val pEditorParamBuilder = Pic2VideoEditorParam.Builder()
            pEditorParamBuilder
                .setMultiPicPath(picturePathList)
                .setTransitionTypeGlsl(CommonUtil.getTransitionFShaderStr(this@JustMediaEditorDemoActivity,
                    TransitionType.fromName(spinnerTransitionType.selectedItem as String)?.value))
                .setFps(FrameRateOption.fromName(spinnerFrameRate.selectedItem as String)?.fps)
                .setWidth(resolutionOption?.width)
                .setHeight(resolutionOption?.height)
                .setBitrate(resolutionOption?.bitRate)
                .setAudioUrl(tvAudioPath.getTextString())
                .setDstUrl(CommonUtil.generateDstUrl())
                .setCoverImage(coverImageBitmap)

                .setStickerImageOptionList(stickerOptionList)
                .setFontTextOptionList(textOptionList)
            if (markImageBitmap != null) {
                pEditorParamBuilder.setMarkImageOption(
                    MarkImageOption(markImageBitmap!!,
                        etMarkImageX.getTextString()?.toFloat() ?: 1F,
                        etMarkImageY.getTextString()?.toFloat() ?: 1F))
            }

            JustPic2VideoEditor.startEditor(this@JustMediaEditorDemoActivity, pEditorParamBuilder.build())
        }
        startActivity(JustMediaEditorProgressActivity.getIntent(this@JustMediaEditorDemoActivity, currentEditorType))
    }

    private fun initStickerRecyclerView() {
        stickerListAdapter = StickerListAdapter(stickerOptionList)
        rcySticker.adapter = stickerListAdapter
        rcySticker.layoutManager = LinearLayoutManager(this@JustMediaEditorDemoActivity)
    }

    private fun initFontTextRecyclerView() {
        fontTextListAdapter = FontTextListAdapter(textOptionList)
        rcyText.adapter = fontTextListAdapter
        rcyText.layoutManager = LinearLayoutManager(this@JustMediaEditorDemoActivity)
    }

    private fun updateEditorParamItemVisible() {
        if (currentEditorType == JustMediaEditorConstant.EDITOR_TYPE_SINGLE_VIDEO) {
            llSelectVideo.visibility = View.VISIBLE
            llSelectBackgroundMusic.visibility = View.VISIBLE
            llChoseFilter.visibility = View.VISIBLE
            llChoseSpeed.visibility = View.VISIBLE
            spinnerResolutionVideo.visibility = View.VISIBLE

            tvSelectFrameRate.visibility = View.GONE
            spinnerFrameRate.visibility = View.GONE
            llSelectMultiPic.visibility = View.GONE
            llChoseTransitionType.visibility = View.GONE
            spinnerResolutionImage.visibility = View.GONE
        } else if (currentEditorType == JustMediaEditorConstant.EDITOR_TYPE_MULTI_IMAGE) {
            llSelectVideo.visibility = View.GONE
            llSelectBackgroundMusic.visibility = View.GONE
            llChoseFilter.visibility = View.GONE
            llChoseSpeed.visibility = View.GONE
            spinnerResolutionVideo.visibility = View.GONE

            tvSelectFrameRate.visibility = View.VISIBLE
            spinnerFrameRate.visibility = View.VISIBLE
            llSelectMultiPic.visibility = View.VISIBLE
            llChoseTransitionType.visibility = View.VISIBLE
            spinnerResolutionImage.visibility = View.VISIBLE
        }
    }

    private fun isVideoEditor(): Boolean = currentEditorType == JustMediaEditorConstant.EDITOR_TYPE_SINGLE_VIDEO
}