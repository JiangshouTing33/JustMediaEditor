#include <jni.h>
#include <string>
#include <JustMediaEditor.h>
#include <JustPic2VideoEditor.h>

#define NATIVE_EDITOR_CLASS_NAME "just/android/mediaeditor/editor/JustEditorNativeBridge"
#ifdef __cplusplus
extern "C" {
#include <libavcodec/version.h>
#include <libavcodec/avcodec.h>
#endif

JNIEXPORT jlong JNICALL native_JustMediaEditor_Init(JNIEnv *env, jobject instance, jstring jSrcVideoUrl, jstring jSrcAudioUrl, jstring jDstUrl) {
    if (jDstUrl == nullptr) {
        return -1;
    }

    const char *dstUrl = env->GetStringUTFChars(jDstUrl, nullptr);
    const char *srcVideoUrl = nullptr;
    const char *srcAudioUrl = nullptr;

    if (jSrcVideoUrl != nullptr) {
        srcVideoUrl = env->GetStringUTFChars(jSrcVideoUrl, nullptr);
    }

    if (jSrcAudioUrl != nullptr) {
        srcAudioUrl = env->GetStringUTFChars(jSrcAudioUrl, nullptr);
    }

    JustMediaEditor *editor = new JustMediaEditor();
    editor->Init(env, instance, srcVideoUrl, srcAudioUrl, dstUrl);

    if (srcVideoUrl) {
        env->ReleaseStringUTFChars(jSrcVideoUrl, srcVideoUrl);
    }
    if (jSrcAudioUrl) {
        env->ReleaseStringUTFChars(jSrcAudioUrl, srcAudioUrl);
    }
    env->ReleaseStringUTFChars(jDstUrl, dstUrl);
    return reinterpret_cast<jlong>(editor);
}

JNIEXPORT void JNICALL native_JustMediaEditor_UnInit(JNIEnv *env, jobject instance, jlong jEditorHandler) {
    if (jEditorHandler != 0) {
        JustMediaEditor *editor = reinterpret_cast<JustMediaEditor *>(jEditorHandler);
        editor->UnInit();
    }
}

JNIEXPORT void JNICALL native_JustMediaEditor_Start(JNIEnv *env, jobject instance, jlong jEditorHandler) {
    if (jEditorHandler != 0) {
        JustMediaEditor *editor = reinterpret_cast<JustMediaEditor *>(jEditorHandler);
        editor->Start();
    }
}

JNIEXPORT void JNICALL native_JustMediaEditor_Pause(JNIEnv *env, jobject instance, jlong jEditorHandler) {
    if (jEditorHandler != 0) {
        JustMediaEditor *editor = reinterpret_cast<JustMediaEditor *>(jEditorHandler);
        editor->Pause();
    }
}

JNIEXPORT void JNICALL native_JustMediaEditor_Stop(JNIEnv *env, jobject instance, jlong jEditorHandler) {
    if (jEditorHandler != 0) {
        JustMediaEditor *editor = reinterpret_cast<JustMediaEditor *>(jEditorHandler);
        editor->Stop();
    }
}

JNIEXPORT void JNICALL native_JustMediaEditor_SetCoverImage(JNIEnv *env, jobject instance,
    jlong jEditorHandler, jint width, jint height, jbyteArray data, jdouble duration) {
    if (jEditorHandler != 0) {
        JustMediaEditor *editor = reinterpret_cast<JustMediaEditor *>(jEditorHandler);
        int length = env->GetArrayLength(data);
        uint8_t *buffer = new uint8_t[length];
        env->GetByteArrayRegion(data, 0, length, reinterpret_cast<jbyte *>(buffer));
        editor->SetCoverImage(width, height, buffer, duration);
        delete[] buffer;
        env->DeleteGlobalRef(data);
    }
}

JNIEXPORT void JNICALL native_JustMediaEditor_SetImageMarkData(JNIEnv *env, jobject instance,
    jlong jEditorHandler, jint width, jint height, jbyteArray data, jfloat markPositionX, jfloat markPositionY) {
    if (jEditorHandler != 0) {
        JustMediaEditor *editor = reinterpret_cast<JustMediaEditor *>(jEditorHandler);
        int length = env->GetArrayLength(data);
        uint8_t *buffer = new uint8_t[length];
        env->GetByteArrayRegion(data, 0, length, reinterpret_cast<jbyte *>(buffer));
        editor->SetImageMarkData(width, height, buffer, markPositionX, markPositionY);
        delete[] buffer;
        env->DeleteGlobalRef(data);
    }
}

JNIEXPORT void JNICALL native_JustMediaEditor_AddTextDisplayParam(JNIEnv *env, jobject instance,
    jlong jEditorHandler, jbyteArray textStr, jstring fontNameStr,
    jlong startTime, jlong endTime, jfloat red, jfloat green, jfloat blue,
    jfloat positionX, jfloat positionY) {

    if (jEditorHandler != 0) {
        JustMediaEditor *editor = reinterpret_cast<JustMediaEditor *>(jEditorHandler);
        const wchar_t *buffer = reinterpret_cast<const wchar_t *>(env->GetByteArrayElements(textStr,
                                                                                            nullptr));
        int n = env->GetArrayLength(textStr) / sizeof(wchar_t);
        std::wstring text(buffer + 1, buffer + n);
        const char *fontName = env->GetStringUTFChars(fontNameStr, nullptr);
        editor->AddTextDisplayParam(&text[0], fontName, startTime, endTime, red, green, blue, positionX, positionY);

        env->ReleaseByteArrayElements(textStr, (jbyte *) buffer, 0);
        env->ReleaseStringUTFChars(fontNameStr, fontName);
    }
}

JNIEXPORT void JNICALL native_JustMediaEditor_AddStickerDisplayOption(JNIEnv *env, jobject instance,
																  jlong jEditorHandler,
																  jint width, jint height, jbyteArray data,
																  jlong startTime, jlong endTime,
																  jfloat positionX, jfloat positionY) {

	if (jEditorHandler != 0) {
		JustMediaEditor *editor = reinterpret_cast<JustMediaEditor *>(jEditorHandler);
		int length = env->GetArrayLength(data);
		uint8_t *buffer = new uint8_t[length];
		env->GetByteArrayRegion(data, 0, length, reinterpret_cast<jbyte *>(buffer));
		editor->AddStickerDisplayOption(width, height, buffer, startTime, endTime, positionX, positionY);
	}
}

JNIEXPORT void JNICALL native_JustMediaEditor_SetLutImageData(JNIEnv *env, jobject instance,
                                                               jlong jEditorHandler, jint width, jint height, jbyteArray data) {
    if (jEditorHandler != 0) {
        JustMediaEditor *editor = reinterpret_cast<JustMediaEditor *>(jEditorHandler);
        int length = env->GetArrayLength(data);
        uint8_t *buffer = new uint8_t[length];
        env->GetByteArrayRegion(data, 0, length, reinterpret_cast<jbyte *>(buffer));
        editor->SetLutImageData(width, height, buffer);
        delete[] buffer;
        env->DeleteGlobalRef(data);
    }
}

JNIEXPORT void JNICALL native_JustMediaEditor_SetAudioFilterData(JNIEnv *env, jobject instance,
                                                              jlong jEditorHandler,
                                                              jstring jFilterDesc, jstring jInputNameOne, jstring jInputNameTwo, jstring jOutputName) {
    if (jEditorHandler != 0) {
        JustMediaEditor *editor = reinterpret_cast<JustMediaEditor *>(jEditorHandler);

        const char *filterDesc = env->GetStringUTFChars(jFilterDesc, nullptr);
        const char *outputName = env->GetStringUTFChars(jOutputName, nullptr);
        const char *inputNameOne = env->GetStringUTFChars(jInputNameOne, nullptr);
        const char *inputNameTwo = nullptr;

        int audioFilterType = AudioFilterType::TYPE_ONE_INPUT;

        if (jInputNameTwo != nullptr) {
            inputNameTwo = env->GetStringUTFChars(jInputNameTwo, nullptr);
            audioFilterType = AudioFilterType::TYPE_TWO_INPUT;
        }

        editor->SetAudioFilterData(audioFilterType, filterDesc, inputNameOne, inputNameTwo, outputName);

        env->ReleaseStringUTFChars(jFilterDesc, filterDesc);
        env->ReleaseStringUTFChars(jOutputName, outputName);
        env->ReleaseStringUTFChars(jInputNameOne, inputNameOne);

        if (inputNameTwo) {
            env->ReleaseStringUTFChars(jInputNameTwo, inputNameTwo);
        }
    }
}

JNIEXPORT void JNICALL native_JustMediaEditor_SetBackgroundMusicData(JNIEnv *env, jobject instance,
                                                              jlong jEditorHandler, jstring jBackgroundAudioUrl) {
    if (jEditorHandler != 0) {
        JustMediaEditor *editor = reinterpret_cast<JustMediaEditor *>(jEditorHandler);

        const char *backgroundAudioUrl = nullptr;
        if (jBackgroundAudioUrl != nullptr) {
            backgroundAudioUrl = env->GetStringUTFChars(jBackgroundAudioUrl, nullptr);
        }

        editor->SetBackgroundMusicData(backgroundAudioUrl);

        if (backgroundAudioUrl) {
            env->ReleaseStringUTFChars(jBackgroundAudioUrl, backgroundAudioUrl);
        }
    }
}

JNIEXPORT void JNICALL native_JustMediaEditor_ChangeVideoSpeed(JNIEnv *env, jobject instance,
                                                                     jlong jEditorHandler, float videoSpeed) {
    if (jEditorHandler != 0) {
        JustMediaEditor *editor = reinterpret_cast<JustMediaEditor *>(jEditorHandler);
        editor->ChangeVideoSpeed(videoSpeed);
    }
}

JNIEXPORT void JNICALL native_JustMediaEditor_SetMuxerParam(JNIEnv *env, jobject instance,
                                                               jlong jEditorHandler, jint jWidth, jint jHeight, jlong jBitrate) {
    if (jEditorHandler != 0) {
        JustMediaEditor *editor = reinterpret_cast<JustMediaEditor *>(jEditorHandler);
        editor->SetMuxerParam(jWidth, jHeight, jBitrate);
    }
}

//Picture -> Video Editor Start
JNIEXPORT jlong JNICALL native_JustPic2VideoEditor_Init(JNIEnv *env, jobject instance, jstring jSrcAudioUrl, jstring jDstUrl, jint jDstWidth, jint jDstHeight, jint jFps, jlong jVideoBitRate) {
    if (jDstUrl == nullptr || jSrcAudioUrl == nullptr) {
        return -1;
    }

    const char *dstUrl = env->GetStringUTFChars(jDstUrl, nullptr);
    const char *srcVideoUrl = nullptr;
    const char *srcAudioUrl = env->GetStringUTFChars(jSrcAudioUrl, nullptr);

    JustPic2VideoEditor *pic2VideoEditor = new JustPic2VideoEditor();
    pic2VideoEditor->Init(env, instance, srcAudioUrl, dstUrl, jDstWidth, jDstHeight, jFps, jVideoBitRate);

    env->ReleaseStringUTFChars(jSrcAudioUrl, srcAudioUrl);
    env->ReleaseStringUTFChars(jDstUrl, dstUrl);
    return reinterpret_cast<jlong>(pic2VideoEditor);
}

JNIEXPORT void JNICALL native_JustPic2VideoEditor_UnInit(JNIEnv *env, jobject instance, jlong jEditorHandler) {
    if (jEditorHandler != 0) {
        JustPic2VideoEditor *editor = reinterpret_cast<JustPic2VideoEditor *>(jEditorHandler);
        editor->UnInit();
    }
}

JNIEXPORT void JNICALL native_JustPic2VideoEditor_Start(JNIEnv *env, jobject instance, jlong jEditorHandler) {
    if (jEditorHandler != 0) {
        JustPic2VideoEditor *editor = reinterpret_cast<JustPic2VideoEditor *>(jEditorHandler);
        editor->Start();
    }
}

JNIEXPORT void JNICALL native_JustPic2VideoEditor_Pause(JNIEnv *env, jobject instance, jlong jEditorHandler) {
    if (jEditorHandler != 0) {
        JustPic2VideoEditor *editor = reinterpret_cast<JustPic2VideoEditor *>(jEditorHandler);
        editor->Pause();
    }
}

JNIEXPORT void JNICALL native_JustPic2VideoEditor_Stop(JNIEnv *env, jobject instance, jlong jEditorHandler) {
    if (jEditorHandler != 0) {
        JustPic2VideoEditor *editor = reinterpret_cast<JustPic2VideoEditor *>(jEditorHandler);
        editor->Stop();
    }
}

JNIEXPORT void JNICALL native_JustPic2VideoEditor_SetCoverImage(JNIEnv *env, jobject instance,
                                                            jlong jEditorHandler, jint width, jint height, jbyteArray data, jdouble duration) {
    if (jEditorHandler != 0) {
        JustPic2VideoEditor *editor = reinterpret_cast<JustPic2VideoEditor *>(jEditorHandler);
        int length = env->GetArrayLength(data);
        uint8_t *buffer = new uint8_t[length];
        env->GetByteArrayRegion(data, 0, length, reinterpret_cast<jbyte *>(buffer));
        editor->SetCoverImage(width, height, buffer, duration);
        delete[] buffer;
        env->DeleteGlobalRef(data);
    }
}

JNIEXPORT void JNICALL native_JustPic2VideoEditor_SetImageMarkData(JNIEnv *env, jobject instance,
                                                               jlong jEditorHandler, jint width, jint height, jbyteArray data, jfloat markPositionX, jfloat markPositionY) {
    if (jEditorHandler != 0) {
        JustPic2VideoEditor *editor = reinterpret_cast<JustPic2VideoEditor *>(jEditorHandler);
        int length = env->GetArrayLength(data);
        uint8_t *buffer = new uint8_t[length];
        env->GetByteArrayRegion(data, 0, length, reinterpret_cast<jbyte *>(buffer));
        editor->SetImageMarkData(width, height, buffer, markPositionX, markPositionY);
        delete[] buffer;
        env->DeleteGlobalRef(data);
    }
}

JNIEXPORT void JNICALL native_JustPic2VideoEditor_AddTextDisplayParam(JNIEnv *env, jobject instance,
                                                                  jlong jEditorHandler, jbyteArray textStr, jstring fontNameStr,
                                                                  jlong startTime, jlong endTime, jfloat red, jfloat green, jfloat blue,
                                                                  jfloat positionX, jfloat positionY) {

    if (jEditorHandler != 0) {
        JustPic2VideoEditor *editor = reinterpret_cast<JustPic2VideoEditor *>(jEditorHandler);
        const wchar_t *buffer = reinterpret_cast<const wchar_t *>(env->GetByteArrayElements(textStr,
                                                                                            nullptr));
        int n = env->GetArrayLength(textStr) / sizeof(wchar_t);
        std::wstring text(buffer + 1, buffer + n);
        const char *fontName = env->GetStringUTFChars(fontNameStr, nullptr);
        editor->AddTextDisplayParam(&text[0], fontName, startTime, endTime, red, green, blue, positionX, positionY);

        env->ReleaseByteArrayElements(textStr, (jbyte *) buffer, 0);
        env->ReleaseStringUTFChars(fontNameStr, fontName);
    }
}

JNIEXPORT void JNICALL native_JustPic2VideoEditor_AddStickerDisplayOption(JNIEnv *env, jobject instance,
                                                                      jlong jEditorHandler,
                                                                      jint width, jint height, jbyteArray data,
                                                                      jlong startTime, jlong endTime,
                                                                      jfloat positionX, jfloat positionY) {

    if (jEditorHandler != 0) {
        JustPic2VideoEditor *editor = reinterpret_cast<JustPic2VideoEditor *>(jEditorHandler);
        int length = env->GetArrayLength(data);
        uint8_t *buffer = new uint8_t[length];
        env->GetByteArrayRegion(data, 0, length, reinterpret_cast<jbyte *>(buffer));
        editor->AddStickerDisplayOption(width, height, buffer, startTime, endTime, positionX, positionY);
    }
}

JNIEXPORT void JNICALL native_JustPic2VideoEditor_SetImageSourceWithIndex(JNIEnv *env, jobject instance,
																		  jlong jEditorHandler, jint index,
																		  jint width, jint height, jbyteArray data) {
	if (jEditorHandler != 0) {
		JustPic2VideoEditor *editor = reinterpret_cast<JustPic2VideoEditor *>(jEditorHandler);
		int length = env->GetArrayLength(data);
		uint8_t *buffer = new uint8_t[length];
		env->GetByteArrayRegion(data, 0, length, reinterpret_cast<jbyte *>(buffer));
		editor->SetImageSourceWithIndex(index, width, height, buffer);
	}
}

JNIEXPORT void JNICALL native_JustPic2VideoEditor_SetImageCount(JNIEnv *env, jobject instance,
																		  jlong jEditorHandler, jint imageCount) {
	if (jEditorHandler != 0) {
		JustPic2VideoEditor *editor = reinterpret_cast<JustPic2VideoEditor *>(jEditorHandler);
		editor->SetImageCount(imageCount);
	}
}

JNIEXPORT void JNICALL native_JustPic2VideoEditor_SetTransitionFShaderStr(JNIEnv *env, jobject instance,
                                                                jlong jEditorHandler, jstring jFShaderStr) {
    if (jEditorHandler != 0) {
        JustPic2VideoEditor *editor = reinterpret_cast<JustPic2VideoEditor *>(jEditorHandler);
		const char *fShaderStr = env->GetStringUTFChars(jFShaderStr, nullptr);
        editor->SetTransitionFShaderStr(fShaderStr);

		env->ReleaseStringUTFChars(jFShaderStr, fShaderStr);
    }
}


#ifdef __cplusplus
}
#endif

static JNINativeMethod g_MediaEditorMethod[] = {
        {"native_JustMediaEditor_Init",             "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)J",       (void *)(native_JustMediaEditor_Init)},
        {"native_JustMediaEditor_UnInit",             "(J)V",       (void *)(native_JustMediaEditor_UnInit)},
        {"native_JustMediaEditor_Start",             "(J)V",       (void *)(native_JustMediaEditor_Start)},
        {"native_JustMediaEditor_Pause",             "(J)V",       (void *)(native_JustMediaEditor_Pause)},
        {"native_JustMediaEditor_Stop",             "(J)V",       (void *)(native_JustMediaEditor_Stop)},
        {"native_JustMediaEditor_SetCoverImage",             "(JII[BD)V",       (void *)(native_JustMediaEditor_SetCoverImage)},
        {"native_JustMediaEditor_SetImageMarkData",             "(JII[BFF)V",       (void *)(native_JustMediaEditor_SetImageMarkData)},
        {"native_JustMediaEditor_AddTextDisplayParam",             "(J[BLjava/lang/String;JJFFFFF)V",       (void *)(native_JustMediaEditor_AddTextDisplayParam)},
		{"native_JustMediaEditor_AddStickerDisplayOption",             "(JII[BJJFF)V",       (void *)(native_JustMediaEditor_AddStickerDisplayOption)},
        {"native_JustMediaEditor_SetLutImageData",             "(JII[B)V",       (void *)(native_JustMediaEditor_SetLutImageData)},
        {"native_JustMediaEditor_SetAudioFilterData",             "(JLjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V", (void *)(native_JustMediaEditor_SetAudioFilterData)},
        {"native_JustMediaEditor_SetBackgroundMusicData",             "(JLjava/lang/String;)V",       (void *)(native_JustMediaEditor_SetBackgroundMusicData)},
        {"native_JustMediaEditor_ChangeVideoSpeed",             "(JF)V",       (void *)(native_JustMediaEditor_ChangeVideoSpeed)},
        {"native_JustMediaEditor_SetMuxerParam",             "(JIIJ)V",       (void *)(native_JustMediaEditor_SetMuxerParam)},

        //Pic2Video Editor Start
        {"native_JustPic2VideoEditor_Init",             "(Ljava/lang/String;Ljava/lang/String;IIIJ)J",       (void *)(native_JustPic2VideoEditor_Init)},
        {"native_JustPic2VideoEditor_UnInit",             "(J)V",       (void *)(native_JustPic2VideoEditor_UnInit)},
        {"native_JustPic2VideoEditor_Start",             "(J)V",       (void *)(native_JustPic2VideoEditor_Start)},
        {"native_JustPic2VideoEditor_Pause",             "(J)V",       (void *)(native_JustPic2VideoEditor_Pause)},
        {"native_JustPic2VideoEditor_Stop",             "(J)V",       (void *)(native_JustPic2VideoEditor_Stop)},
        {"native_JustPic2VideoEditor_SetCoverImage",             "(JII[BD)V",       (void *)(native_JustPic2VideoEditor_SetCoverImage)},
        {"native_JustPic2VideoEditor_SetImageMarkData",             "(JII[BFF)V",       (void *)(native_JustPic2VideoEditor_SetImageMarkData)},
        {"native_JustPic2VideoEditor_AddTextDisplayParam",             "(J[BLjava/lang/String;JJFFFFF)V",       (void *)(native_JustPic2VideoEditor_AddTextDisplayParam)},
        {"native_JustPic2VideoEditor_AddStickerDisplayOption",             "(JII[BJJFF)V",       (void *)(native_JustPic2VideoEditor_AddStickerDisplayOption)},
		{"native_JustPic2VideoEditor_SetImageSourceWithIndex",             "(JIII[B)V",       (void *)(native_JustPic2VideoEditor_SetImageSourceWithIndex)},
		{"native_JustPic2VideoEditor_SetImageCount",             "(JI)V",       (void *)(native_JustPic2VideoEditor_SetImageCount)},
        {"native_JustPic2VideoEditor_SetTransitionFShaderStr",             "(JLjava/lang/String;)V",       (void *)(native_JustPic2VideoEditor_SetTransitionFShaderStr)},

};

static int RegisterNativeMethods(JNIEnv *env, const char *className, JNINativeMethod *nativeMethod, int methodNumber) {
    jclass clazz = env->FindClass(className);

    if (clazz == nullptr) {
        return JNI_FALSE;
    }

    if (env->RegisterNatives(clazz, nativeMethod, methodNumber) < 0) {
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

static void UnRegisterNativeMethod(JNIEnv *env, const char *className) {
    jclass clazz = env->FindClass(className);

    if (clazz == nullptr) {
        return;
    }

    env->UnregisterNatives(clazz);
}

extern "C" jint JNI_OnLoad(JavaVM *jvm, void *p) {
    jint jniRet = JNI_ERR;
    JNIEnv *env = nullptr;

    if (jvm->GetEnv((void **)(&env), JNI_VERSION_1_6) != JNI_OK) {
        return jniRet;
    }

    jint regRes = RegisterNativeMethods(env, NATIVE_EDITOR_CLASS_NAME, g_MediaEditorMethod,
                                        sizeof(g_MediaEditorMethod) / sizeof(g_MediaEditorMethod[0]));

    if (regRes != JNI_TRUE) {
        return jniRet;
    }

    return JNI_VERSION_1_6;
}

JNIEXPORT void JNI_OnUnload(JavaVM* vm, void* reserved) {
    JNIEnv *env = nullptr;

    if (vm->GetEnv((void **)(&env), JNI_VERSION_1_6) != JNI_OK) {
        return;
    }

    UnRegisterNativeMethod(env, NATIVE_EDITOR_CLASS_NAME);
}