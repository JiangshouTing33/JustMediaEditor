package just.android.mediaeditor.constant

object EditorDefaultParam {
    const val AUDIO_MIX_FILTER_DESC =
        "[in0]volume=1.0[a0];\n" +
                "[in1]volume=0.8[a1];\n" +
                "[a0][a1]amix=inputs=2[out]"

    const val AUDIO_MIX_FILTER_WITH_ATEMPO_DESC =
        "[in0]volume=1.0[a0];\n" +
                "[in1]volume=0.8[a1];\n" +
                "[a0][a1]amix=inputs=2[a3];\n" +
                "[a3]atempo=%s[out]"

    const val AUDIO_MIX_FILTER_WITH_4_ATEMPO_DESC =
        "[in0]volume=1.0[a0];\n" +
                "[in1]volume=0.8[a1];\n" +
                "[a0][a1]amix=inputs=2[a3];\n" +
                "[a3]atempo=2, atempo=2[out]"

    const val AUDIO_TEMPO_FILTER_DESC = "[in0]atempo=%s[out]"

    const val AUDIO_4_TEMPO_FILTER_DESC = "[in0]atempo=2, atempo=2[out]"

    const val AUDIO_INPUT_ONE_NAME = "in0"
    const val AUDIO_INPUT_TWO_NAME = "in1"
    const val AUDIO_OUTPUT_NAME = "out"

    const val DEFAULT_COVER_IMAGE_DURATION = 3.0
}