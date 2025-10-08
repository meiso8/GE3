#pragma once
#include<vector>

class Texture {
public:
    static Texture* GetInstance();
    enum TAG {
        WHITE_1X1,
        UV_CHECKER,
        NUMBERS,
        PLAYER,
        TEXTURES
    };
    static void Load();
    static uint32_t GetHandle(uint32_t tag) {
        if (tag > TEXTURES) {
            return 0;
        }
        return textureHandle_
            [tag];
    };
private:
    static Texture* instance_;
    static std::vector<uint32_t>textureHandle_;
};

