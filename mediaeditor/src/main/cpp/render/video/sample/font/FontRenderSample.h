//
// Created by 江守亭 on 2021/10/11.
//

#ifndef JUSTMEDIAEDITOR_FONTRENDERSAMPLE_H
#define JUSTMEDIAEDITOR_FONTRENDERSAMPLE_H

#include <BaseSample.h>
#include <map>
#include <queue>
#include <TextDisplayParam.h>
#include "ft2build.h"
#include <freetype/ftglyph.h>

static const int MAX_SHORT_VALUE = 65536;
struct Character {
	GLuint texture;
	vec2 size;
	vec2 bearing;
	GLuint advance;
};

class FontRenderSample : public BaseSample {
public:
	FontRenderSample();
	~FontRenderSample();

	virtual int Init();
	virtual void UnInit();
	virtual void Draw();

	void LoadFontFace(std::deque<TextDisplayParam*> displayTextQueue);
	void SetTextDisplayParams(std::deque<TextDisplayParam*> textDisplayParams);

private:
	void LoadFontFace(const char *fontName, const wchar_t *text);
	void RenderText(TextDisplayParam *option, glm::vec2 viewport);

private:
	std::deque<TextDisplayParam*> m_TextDisplayParams;
	std::map<std::string, Character> m_Characters;
	GLuint m_FontProgramId;
	GLuint m_FontVaoId;
	GLuint m_FontVboId;
};


#endif //JUSTMEDIAEDITOR_FONTRENDERSAMPLE_H
