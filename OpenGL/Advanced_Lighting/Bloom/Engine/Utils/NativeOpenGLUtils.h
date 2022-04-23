#pragma once

#include "../Basic/Core.h"

namespace NativeOpenGL {

	struct HdrBuffer {
		unsigned int FrameBuffer;
		unsigned int RenderBuffer;
		std::vector<unsigned int> ColorBuffer;
	};

	struct GaussianBlurBuffer {
		unsigned int FrameBuffer;
		unsigned int BlurBuffer;
	};

	std::pair<unsigned int, unsigned int> GenDataBuffer(float Data[], int Offset[], int Num[], unsigned int DataSize, int CellLen, int OffsetLen) {
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

	std::tuple<unsigned int, unsigned int, unsigned int> GenFrameBuffer(const unsigned int Width, const unsigned int Height) {
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

	HdrBuffer GenFrameBufferHDR(const unsigned int Width, const unsigned int Height, const size_t ColorBufferNum) {
		unsigned int FrameBuffer;
		glGenFramebuffers(1, &FrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
		// Create Color Attachment Texture
		std::vector<unsigned int> TextureColorBuffer(ColorBufferNum);
		for (size_t i = 0; i < ColorBufferNum; ++i) {
			glGenTextures(ColorBufferNum, &TextureColorBuffer[i]);
			glBindTexture(GL_TEXTURE_2D, TextureColorBuffer[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Width, Height, 0, GL_RGBA, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, TextureColorBuffer[i], 0);
		}
		// Create Render Buffer(Depth, Stencil Test)
		unsigned int RenderBuffer;
		glGenRenderbuffers(1, &RenderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, RenderBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Width, Height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RenderBuffer);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			Logger::GetLogger()->PrintError("FrameBuffer Is Not Complete!");
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			return { 0, 0, {} };
		}
		unsigned int* Attachments = new unsigned int[ColorBufferNum];
		for (size_t i = 0; i < ColorBufferNum; ++i)
			Attachments[i] = GL_COLOR_ATTACHMENT0 + i;
		glDrawBuffers(ColorBufferNum, Attachments);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		delete[] Attachments;
		return { FrameBuffer, RenderBuffer, std::move(TextureColorBuffer) };
	}

	GaussianBlurBuffer GenFrameBufferBlur(const unsigned int Width, const unsigned int Height) {
		unsigned int FrameBuffer;
		glGenFramebuffers(1, &FrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
		// Create Color Attachment Texture
		unsigned int BlurBuffer;
		glGenTextures(1, &BlurBuffer);
		glBindTexture(GL_TEXTURE_2D, BlurBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Width, Height, 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, BlurBuffer, 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			Logger::GetLogger()->PrintError("FrameBuffer Is Not Complete!");
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			return { 0, 0 };
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return { FrameBuffer, BlurBuffer };
	}

	std::pair<unsigned int, unsigned int> GenFrameBuffer() {
		const unsigned int Shadow_Width = 1024, Shadow_Height = 1024;
		unsigned int FrameBuffer;
		glGenFramebuffers(1, &FrameBuffer);
		// Create Depth Buffer
		unsigned int DepthBuffer;
		glGenTextures(1, &DepthBuffer);
		glBindTexture(GL_TEXTURE_2D, DepthBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, Shadow_Width, Shadow_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// Attach Depth Buffer
		glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthBuffer, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			Logger::GetLogger()->PrintError("FrameBuffer Is Not Complete!");
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			return { 0, 0 };
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return { FrameBuffer, DepthBuffer };
	}

};
