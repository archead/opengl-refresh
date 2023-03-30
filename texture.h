#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <glad/glad.h>
#include <cstring>

class Texture
{
private:
    int width, height, nrChannels;
public:
    unsigned int ID;
    unsigned int activeTextureOffset;
    Texture(const char* imagePath, unsigned int activeTextureOffset)
    {
        // automatically determine if the image needs an alpha channel for transparency i.e. .png
        unsigned int colorChannel = GL_RGB;
        if (strstr(imagePath, ".png") != NULL)
        {
            colorChannel = GL_RGBA;
        }

        this->activeTextureOffset = activeTextureOffset;
        stbi_set_flip_vertically_on_load(true);
        glGenTextures(1, &ID);
        glActiveTexture(GL_TEXTURE0 + activeTextureOffset);
        glBindTexture(GL_TEXTURE_2D, ID);
        // set the texture wrapping/filtering options (on the currently bound texture object)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        unsigned char* data = stbi_load(imagePath, &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, colorChannel, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else std::cout << "Failed to load texture" << std::endl;
        stbi_image_free(data);
    }

    void SetSampler2D(unsigned int shaderID, const char* samplerVariableName)
    {
        glUniform1i(glGetUniformLocation(shaderID, samplerVariableName), activeTextureOffset);
    }
};