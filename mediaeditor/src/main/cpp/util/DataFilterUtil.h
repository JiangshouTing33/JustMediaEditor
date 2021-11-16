//
// Created by 江守亭 on 2021/10/11.
//

#ifndef JUSTMEDIAEDITOR_DATAFILTERUTIL_H
#define JUSTMEDIAEDITOR_DATAFILTERUTIL_H

#include <TextDisplayParam.h>
#include <StickerRenderSample.h>
#include <queue>

class DataFilterUtil {
public:
	//Font
	static bool TextDisplayParamFilter(TextDisplayParam *param);
	static void SortTextDisplayParams(std::deque<TextDisplayParam*> *params);

	//Sticker
	static bool StickerDisplayOptionFilter(const StickerDisplayOption& option);
	static void SortStickerDisplayOptions(std::deque<StickerDisplayOption> *options);

private:
	static bool TextDisplayParamCompare(const TextDisplayParam *param1, const TextDisplayParam *param2);
	static bool StickerDisplayOptionCompare(const StickerDisplayOption& option1, const StickerDisplayOption& option2);
};


#endif //JUSTMEDIAEDITOR_DATAFILTERUTIL_H
