#pragma once
#include<string>
#include <iostream>

struct MaterialData {
    std::string textureFilePath;
};

MaterialData LoadMaterialTemplateFile(const std::string& dirextoryPath, const std::string& filename);