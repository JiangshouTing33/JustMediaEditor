package just.android.justmediaeditor.adapter

import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ImageView
import android.widget.TextView
import androidx.recyclerview.widget.RecyclerView
import just.android.justmediaeditor.R
import just.android.mediaeditor.param.StickerImageOption

class StickerListAdapter(private val stickerOptionList: MutableList<StickerImageOption>): RecyclerView.Adapter<StickerListAdapter.StickerViewHolder>() {

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): StickerViewHolder =
        StickerViewHolder.create(parent)

    override fun onBindViewHolder(holder: StickerViewHolder, position: Int) {
        holder.bind(stickerOptionList[position])
        holder.tvStickerItemDelete.setOnClickListener {
            stickerOptionList.removeAt(position)
            notifyDataSetChanged()
        }
    }

    override fun getItemCount(): Int = stickerOptionList.size

    class StickerViewHolder(itemView: View) : RecyclerView.ViewHolder(itemView) {
        private var imStickerItemImage: ImageView = itemView.findViewById(R.id.im_sticker_item_image)
        private var tvStickerItemStart: TextView = itemView.findViewById(R.id.tv_sticker_item_start)
        private var tvStickerItemEnd: TextView = itemView.findViewById(R.id.tv_sticker_item_end)
        var tvStickerItemDelete: TextView = itemView.findViewById(R.id.tv_sticker_item_delete)

        companion object {
            fun create(parent: ViewGroup): StickerViewHolder = StickerViewHolder(LayoutInflater.from(parent.context)
                .inflate(R.layout.adapter_item_sticker, parent, false))
        }

        fun bind(stickerImageOption: StickerImageOption) {
            stickerImageOption.let {
                imStickerItemImage.setImageBitmap(it.bitmap)
                tvStickerItemStart.text = String.format(itemView.context.getString(R.string.duration_start), it.durationStart)
                tvStickerItemEnd.text = String.format(itemView.context.getString(R.string.duration_end), it.durationEnd)
            }
        }
    }
}