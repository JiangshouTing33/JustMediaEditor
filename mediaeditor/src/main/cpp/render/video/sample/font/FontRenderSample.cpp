//
// Created by 江守亭 on 2021/10/11.
//
#include "FontRenderSample.h"
FontRenderSample::FontRenderSample() {
	m_FontProgramId = GL_NONE;
	m_FontVaoId = GL_NONE;
	m_FontVboId = GL_NONE;
}

FontRenderSample::~FontRenderSample() = default;

char vFontShaderStr[] =
	"#version 300 es\n"
	"layout(location = 0) in vec4 a_position;// <vec2 pos, vec2 tex>\n"
	"out vec2 v_texCoord;\n"
	"void main()\n"
	"{\n"
	"    gl_Position = vec4(a_position.xy, 0.0, 1.0);\n"
	"    v_texCoord = a_position.zw;\n"
	"}";

const char fFontShaderStr[] =
	"#version 300 es\n"
	"precision mediump float;\n"
	"in vec2 v_texCoord;\n"
	"layout(location = 0) out vec4 outColor;\n"
	"uniform sampler2D s_textTexture;\n"
	"uniform vec3 u_textColor;\n"
	"\n"
	"void main()\n"
	"{\n"
	"    vec4 color = vec4(1.0, 1.0, 1.0, texture(s_textTexture, v_texCoord).r);\n"
	"    outColor = vec4(u_textColor, 1.0) * color;\n"
	"}";

int FontRenderSample::Init() {
	int result = -1;
	if (m_FontProgramId != GL_NONE) {
		result = 0;
		return result;
	}
	m_FontProgramId = GLUtil::CreateProgram(vFontShaderStr, fFontShaderStr);

	if (m_FontProgramId == GL_NONE) {
		LOGCATE("FontRenderSample::Init()..CreateFontProgramId Failed");
		result = -1;
		return result;
	}

	glGenVertexArrays(1, &m_FontVaoId);
	glGenBuffers(1, &m_FontVboId);

	glBindVertexArray(m_FontVaoId);
	glBindBuffer(GL_ARRAY_BUFFER, m_FontVboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	glBindVertexArray(GL_NONE);

	return 0;
}

void FontRenderSample::UnInit() {
	if (m_FontProgramId != GL_NONE) {
		GLUtil::DeleteProgram(m_FontProgramId);
		m_FontProgramId = GL_NONE;
	}

	if (m_FontVaoId != GL_NONE) {
		glDeleteVertexArrays(1, &m_FontVaoId);
		m_FontVaoId = GL_NONE;
	}

	if (m_FontVboId != GL_NONE) {
		glDeleteBuffers(1, &m_FontVboId);
		m_FontVboId = GL_NONE;
	}
}

void FontRenderSample::LoadFontFace(std::deque<TextDisplayParam *> displayTextQueue) {
	for (int i = 0; i < displayTextQueue.size(); ++i) {
		TextDisplayParam *displayParam = displayTextQueue[i];
		LoadFontFace(displayParam->fontName, displayParam->text);
	}
}

void FontRenderSample::SetTextDisplayParams(std::deque<TextDisplayParam *> textDisplayParams) {
	m_TextDisplayParams = std::move(textDisplayParams);
}

void FontRenderSample::Draw() {
	if (m_FontProgramId == GL_NONE || m_TextDisplayParams.empty()) return;
	for (int i = 0; i < m_TextDisplayParams.size(); ++i) {
		RenderText(m_TextDisplayParams.at(i), m_DstSize);
	}
	m_TextDisplayParams.clear();
}

void FontRenderSample::LoadFontFace(const char *fontName, const wchar_t *text) {
	FT_Library ft;
	FT_Face face = nullptr;
	do {
		if (FT_Init_FreeType(&ft)) {
			LOGCATE("FontRenderSample::LoadFontFace Failed: Could not init FreeType Library");
			break;
		}

		if (FT_New_Face(ft, fontName, 0, &face)) {
			LOGCATE("FontRenderSample::LoadFontFace Failed: Could not New Face");
			break;
		}

		FT_Set_Pixel_Sizes(face, 96, 96);
		FT_Select_Charmap(face, ft_encoding_unicode);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		int size = wcslen(text);

		for (int i = 0; i < size; ++i) {
			if (FT_Load_Glyph(face, FT_Get_Char_Index(face, text[i]), FT_LOAD_RENDER)) {
				LOGCATE("FontRenderSample::LoadFontFace Load Glyph failed");
				continue;
			}

			FT_Glyph glyph;
			FT_Get_Glyph(face->glyph, &glyph);

			GLuint textureId = GL_NONE;
			glGenTextures(1, &textureId);
			glBindTexture(GL_TEXTURE_2D, textureId);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, face->glyph->bitmap.width, face->glyph->bitmap.rows,
						 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

			Character character = Character {
				textureId,
				vec2 (face->glyph->bitmap.width, face->glyph->bitmap.rows),
				vec2 (face->glyph->bitmap_left, face->glyph->bitmap_top),
				static_cast<GLuint>((glyph->advance.x / MAX_SHORT_VALUE) << 6)
			};

			char textName[256];
			sprintf(textName, "%s_%d", fontName, text[i]);
			std::string nameKey(textName, textName + strlen(textName));
//            LOGCATD("FontRenderSample::LoadFontFace Load Glyph Success, nameKey = %s, textureId = %d ", nameKey.c_str(), textureId);
			m_Characters.insert(std::pair<std::string, Character>(nameKey, character));
			glBindTexture(GL_TEXTURE_2D, GL_NONE);
		}
	} while (false);
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

void FontRenderSample::RenderText(TextDisplayParam *option, glm::vec2 viewport) {
	if (m_FontProgramId == GL_NONE) return;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glUseProgram(m_FontProgramId);
	glBindVertexArray(m_FontVaoId);

	GLUtil::setVec3(m_FontProgramId, "u_textColor", option->textColor);

	vec2 pos = option->displayPosition;
	pos.x *= viewport.x;
	pos.y *= viewport.y;

	const wchar_t *text = option->text;
	int len = wcslen(text);

	for (int i = 0; i < len; ++i) {
		char textName[256];
		sprintf(textName, "%s_%d", option->fontName, text[i]);
		std::string nameKey(textName, textName + strlen(textName));
		auto iter = m_Characters.find(nameKey);
		if (iter == m_Characters.end()) {
			LOGCATE("FontRenderSample::RenderText..cant find character key = %s", nameKey.c_str());
			continue;
		}
		Character character = iter->second;
//        LOGCATD("FontRenderSample::RenderText..character[textureId, textSize.x, textSize.y, textBearing.x, textBearing.y, advance] = [%d, %f, %f, %f, %f, %d]",
//        character.texture,
//        character.size.x, character.size.y,
//        character.bearing.x, character.bearing.y,
//        character.advance);

		GLfloat xpos = pos.x + character.bearing.x * option->scale;
		GLfloat ypos = pos.y + (character.size.y - character.bearing.y) * option->scale;

		xpos /= viewport.x;
		ypos /= viewport.y;

		GLfloat w = character.size.x * option->scale;
		GLfloat h = -character.size.y * option->scale;

		w /= viewport.x;
		h /= viewport.y;

		GLfloat vertices[6][4] = {
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 },

			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 }
		};

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, character.texture);
		GLUtil::setInt(m_FontProgramId, "s_textTexture", 0);
		glBindBuffer(GL_ARRAY_BUFFER, m_FontVboId);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
		pos.x += (character.advance >> 6) * option->scale;
	}
	glDisable(GL_BLEND);
	glBindVertexArray(GL_NONE);
}