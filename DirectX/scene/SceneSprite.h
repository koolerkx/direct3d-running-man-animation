#pragma once
#include <DirectXMath.h>
#include <vector>

struct SpriteAction
{
    double duration; // 変化時間
    float opacity; // 総変化量
    DirectX::XMMATRIX trasnMat; // 総変化量

    SpriteAction(int duration, float opacity, DirectX::XMMATRIX trasnMat): duration(duration), opacity(opacity),
                                                                           trasnMat(trasnMat)
    {
    };
};

struct TransProps
{
    DirectX::XMFLOAT2 position = {0, 0};
    DirectX::XMFLOAT2 size = {256, 256};
    DirectX::XMFLOAT4 color = {1, 1, 1, 1};
    float angle = 0;
};

class Sprite
{
public:
    Sprite();
    Sprite(const wchar_t* pFilename);
    void draw(double offset);


    // chain method
    Sprite* init(TransProps props);

    Sprite* delay(double duration);
    Sprite* fadeInOut(float from, float to, double duration);
    Sprite* move(DirectX::XMFLOAT2 from, DirectX::XMFLOAT2 to, double duration);

private:
    int textureId;
    double totalDuration;

    TransProps initialStatus;

    std::vector<SpriteAction> actions;
};
