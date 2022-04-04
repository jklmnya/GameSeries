#pragma once

#include "../Basic/Core.h"

namespace NativeOpenGL {

	std::pair<unsigned int, unsigned int> GenBufferWithAB(float Data[], int Offset[], int Num[], unsigned int DataSize, int CellLen, int OffsetLen) {
		unsigned int VAO, VBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, DataSize, Data, GL_STATIC_DRAW);
		for (int i = 0; i < OffsetLen; ++i) {
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i, Num[i], GL_FLOAT, GL_FALSE, CellLen * sizeof(float), (void*)(Offset[i] * sizeof(float)));
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		return std::make_pair(VAO, VBO);
	}

	std::tuple<unsigned int, unsigned int, unsigned int> GenFrameBufferWithCR(const unsigned int Width, const unsigned int Height) {
		unsigned int FrameBuffer;
		glGenFramebuffers(1, &FrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
		// Create Color Attachment Texture
		unsigned int TextureColorBuffer;
		glGenTextures(1, &TextureColorBuffer);
		glBindTexture(GL_TEXTURE_2D, TextureColorBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureColorBuffer, 0);
		// Create Render Buffer(Depth, Stencil Test)
		unsigned int RenderBuffer;
		glGenRenderbuffers(1, &RenderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, RenderBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Width, Height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RenderBuffer);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			Logger::GetLogger()->PrintError("FrameBuffer Is Not Complete!");
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			return { 0, 0, 0 };
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return { FrameBuffer, TextureColorBuffer, RenderBuffer };
	}
};
