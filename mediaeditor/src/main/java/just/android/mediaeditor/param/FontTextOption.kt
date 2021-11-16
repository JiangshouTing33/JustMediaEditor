package just.android.mediaeditor.param

class FontTextOption {
    constructor(text: String,
                fontPath: String,
                positionX: Float,
                positionY: Float,
                durationStart: Float,
                durationEnd: Float,
                colorR: Float,
                colorG: Float,
                colorB: Float) {
        this.text = text
        this.fontPath = fontPath
        this.positionX = positionX
        this.positionY = positionY
        this.durationStart = durationStart
        this.durationEnd = durationEnd
        this.colorR = colorR
        this.colorG = colorG
        this.colorB = colorB
    }

    val text: String
    var fontPath: String

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

    var colorR: Float
    get() {
        return when {
            field <= 0 -> {
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

    var colorG: Float
        get() {
            return when {
                field <= 0 -> {
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

    var colorB: Float
        get() {
            return when {
                field <= 0 -> {
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