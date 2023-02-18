#pragma once
#ifndef GROUND_H
#define GROUND_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
using namespace cv;
using namespace std;

bool loadGround(
	const char* path,
	std::vector<glm::vec3>& out_vertices,
	std::vector<glm::vec2>& out_uvs,
	std::vector<glm::vec3>& out_normals
) {
	Mat img = imread(path);
	int imgType = img.type();
	Mat resImg = Mat(img.rows, img.cols, imgType);
	resize(img, resImg, resImg.size(), 0, 0, INTER_LINEAR);
	Mat gImg = Mat(img.rows, img.cols, CV_8UC1);
	cv::cvtColor(resImg, gImg, cv::COLOR_BGR2GRAY);
	int imgWidth = resImg.rows;
	int imgHeihgt = resImg.cols;
	float verticeScale = 0.1f;
	float ZScale = 5.0f;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;
	glm::vec3 vertex;
	glm::vec2 uv;
	for (int i = 0; i < imgWidth; i++)
	{
		for (int j = 0; j < imgHeihgt; j++)
		{
			vertex[0] = verticeScale * (float)i;
			int c = (int)gImg.at<uchar>(i, j);
			vertex[1] = -ZScale * (float)c / 255.0f;
			vertex[2] = verticeScale * (float)-j;
			temp_vertices.push_back(vertex);
			vertex = glm::vec3 (0.0f, 0.0f, 0.0f);
			temp_normals.push_back(vertex);
			uv[0] = (float)i / (float)(imgWidth);
			uv[1] = (float)j / (float)(imgHeihgt);
			temp_uvs.push_back(uv);
		}
	}
	std::vector<unsigned int> indices;
	for (int i = 0; i < imgHeihgt - 1; i++)
	{
		for (int j = 0; j < imgWidth - 1; j++)
		{
			indices.push_back(i * imgWidth + j);
			indices.push_back(i * imgWidth + j + 1);
			indices.push_back(i * imgWidth + j + imgWidth);
			indices.push_back(i * imgWidth + j + imgWidth);
			indices.push_back(i * imgWidth + j + 1);
			indices.push_back(i * imgWidth + j + imgWidth + 1);
		}
	}
	glm::vec3 normal;
	for (size_t i = 0; i < (imgWidth - 1) * (imgHeihgt - 1) * 6; i += 3)
	{
		unsigned int pIndex1 = indices[i];
		unsigned int pIndex2 = indices[i + 1];
		unsigned int pIndex3 = indices[i + 2];
		glm::vec3 a1 = temp_vertices[pIndex1];
		glm::vec3 a2 = temp_vertices[pIndex2];
		glm::vec3 a3 = temp_vertices[pIndex3];
		
		glm::vec3 a12 = a2 - a1;
		glm::vec3 a13 = a3 - a1;
		
		normal = glm::normalize(glm::cross(a13, a12));
		
		temp_normals[pIndex1] += normal;
		temp_normals[pIndex2] += normal;
		temp_normals[pIndex3] += normal;
	}

	for (int i = 0; i < imgWidth - 1; i++)
	{
		for (int j = 0; j < imgHeihgt - 1; j++)
		{
			out_vertices.push_back(temp_vertices[i * imgHeihgt + j]);
			out_vertices.push_back(temp_vertices[i * imgHeihgt + j + 1]);
			out_vertices.push_back(temp_vertices[i * imgHeihgt + j + imgHeihgt]);
			out_vertices.push_back(temp_vertices[i * imgHeihgt + j + imgHeihgt]);
			out_vertices.push_back(temp_vertices[i * imgHeihgt + j + 1]);
			out_vertices.push_back(temp_vertices[i * imgHeihgt + j + imgHeihgt + 1]);
			out_uvs.push_back(temp_uvs[i * imgHeihgt + j]);
			out_uvs.push_back(temp_uvs[i * imgHeihgt + j + 1]);
			out_uvs.push_back(temp_uvs[i * imgHeihgt + j + imgHeihgt]);
			out_uvs.push_back(temp_uvs[i * imgHeihgt + j + imgHeihgt]);
			out_uvs.push_back(temp_uvs[i * imgHeihgt + j + 1]);
			out_uvs.push_back(temp_uvs[i * imgHeihgt + j + imgHeihgt + 1]);
			out_normals.push_back(temp_normals[i * imgHeihgt + j]);
			out_normals.push_back(temp_normals[i * imgHeihgt + j + 1]);
			out_normals.push_back(temp_normals[i * imgHeihgt + j + imgHeihgt]);
			out_normals.push_back(temp_normals[i * imgHeihgt + j + imgHeihgt]);
			out_normals.push_back(temp_normals[i * imgHeihgt + j + 1]);
			out_normals.push_back(temp_normals[i * imgHeihgt + j + imgHeihgt + 1]);
		}
	}
	if (out_vertices.size() > 0) return true;
	else return false;
}
#endif