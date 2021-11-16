package just.android.mediaeditor.param

import android.graphics.Bitmap

class StickerImageOption {
    constructor(bitmap: Bitmap, positionX: Float, positionY: Float, durationStart: Float, durationEnd: Float){
        this.bitmap = bitmap
        this.positionX = positionX
        this.positionY = positionY
        this.durationStart = durationStart
        this.durationEnd = durationEnd
    }

    val bitmap: Bitmap
    var positionX: Float
        get() {
            return when {
                field <= -1F -> {
                    -1F
                }
                field >= 1F -> {
                    1F
                }
                else -> {
                    field
                }
            }
        }
    var positionY: Float
        get() {
            return when {
                field <= -1F -> {
                    -1F
                }
                field >= 1F -> {
                    1F
                }
                else -> {
                    field
                }
            }
        }

    var durationStart: Float
        get() {
            return when {
                field < 0 -> 0F
                else -> field
            }
        }

    var durationEnd: Float
        get() {
            return when {
                field < 0 -> 0F
                else -> field
            }
        }
}