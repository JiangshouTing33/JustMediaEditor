package just.android.mediaeditor.param

import android.graphics.Bitmap

class MarkImageOption {
    constructor(bitmap: Bitmap, positionX: Float, positionY: Float){
        this.bitmap = bitmap
        this.positionX = positionX
        this.positionY = positionY
    }

    val bitmap: Bitmap
    var positionX: Float
    get() {
        return when {
            field <= 0F -> {
                0F
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
                field <= 0F -> {
                    0F
                }
                field >= 1F -> {
                    1F
                }
                else -> {
                    field
                }
            }
        }
}