#pragma once
#include<vector>
#include<stdint.h>

class Texture {
public:
    static Texture* GetInstance();
    

    enum TAG {
        WHITE_1X1,
        UV_CHECKER,
        NUMBERS,
        PLAYER,
        TEXTURE_MAX
    };
    static void Load();
    static uint32_t GetHandle(TAG tag) {
        if (tag > TEXTURE_MAX) {
            return 0;
        }
        return textureHandle_
            [tag];
    };
private:
    static Texture* instance_;
    static std::vector<uint32_t>textureHandle_;
};

