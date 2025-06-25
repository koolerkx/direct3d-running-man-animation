#pragma once

class Sprite
{
public:
    Sprite();
    Sprite(const wchar_t* pFilename);
    void draw(int offset);

private:
    int textureId;
};
