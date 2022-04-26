#pragma once

#include "../Basic/Core.h"

namespace StringUtil {

	inline Vector3 StringToVec3f(const std::string& Line) {
		Vector3 res(0.0f);
		size_t s = 0, n = Line.size(), idx = 0;
		while (s < n) {
			size_t e = s;
			while (e < n && Line[e] != ' ')
				++e;
			res[idx++] = stof(Line.substr(s, e - s));
			s = e + 1;
		}
		return res;
	}

	inline Mat3 StringToMat3f(const std::string& Line) {
		Mat3 res;
		size_t s = 0, n = Line.size(), idx = 0;
		while (s < n) {
			size_t e = s;
			while (e < n && Line[e] != ' ')
				++e;
			res[idx / 3][idx % 3] = stof(Line.substr(s, e - s));
			++idx;
			s = e + 1;
		}
		return res;
	}

	std::vector<Vector3> PRTLightToSH(const char* Path) {
		std::fstream File;
		File.open(Path);
		std::vector<Vector3> res;
		if (File.is_open()) {
			std::string Line;
			getline(File, Line);
			res.reserve(stoi(Line));
			while (getline(File, Line))
				res.push_back(std::move(StringToVec3f(Line)));
		}
		else {
			Logger::GetLogger()->PrintError("Can Not Open File");
		}
		File.close();
		return res;
	}

	std::vector<Mat3> PRTLightTransferToSH(const char* Path) {
		std::fstream File;
		File.open(Path);
		std::vector<Mat3> res;
		if (File.is_open()) {
			std::string Line;
			getline(File, Line);
			res.reserve(stoi(Line));
			while (getline(File, Line))
				res.push_back(std::move(StringToMat3f(Line)));
		}
		else {
			Logger::GetLogger()->PrintError("Can Not Open File");
		}
		File.close();
		return res;
	}
};
