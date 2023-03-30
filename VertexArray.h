#pragma once
#include <glad/glad.h>

class VertexArray 
{
public:
	unsigned int ID;
	VertexArray()
	{
		glGenVertexArrays(1, &ID);
		glBindVertexArray(ID);
	}
	void bind()
	{
		glBindVertexArray(ID);
	}
	void unbind()
	{
		glBindVertexArray(0);
	}
};