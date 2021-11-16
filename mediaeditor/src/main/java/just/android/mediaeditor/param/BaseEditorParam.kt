package just.android.mediaeditor.param

import android.graphics.Bitmap

abstract class BaseEditorParam(var width: Int?,
                               var height: Int?,
                               var bitRate: Long?,
                               var audioUrl: String?,
                               var dstUrl: String,
                               var coverImage: Bitmap?,
                               var markImageOption: MarkImageOption?,
                               val stickerOptionList: List<StickerImageOption>?,
                               val textOptionList: List<FontTextOption>?)