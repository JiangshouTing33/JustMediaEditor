package just.android.mediaeditor.editor

interface IEditEventListener {
    fun onEditEvent(messageType: MediaEditorMessageType?, messageValue: Float)
}