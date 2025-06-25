#include "SceneSprite.h"

#include "debug_ostream.h"
#include "sprite.h"
#include "sprite_anim.h"
#include "texture.h"

Sprite::Sprite(): textureId(-1)
{
}

Sprite::Sprite(const wchar_t* pFilename)
{
    textureId = Texture_Load(pFilename);
}

Sprite* Sprite::init(TransProps props)
{
    initialStatus = props;

    return this;
}

void Sprite::draw(double offset)
{
    double durationApplied = 0.0;

    DirectX::XMFLOAT4 color = initialStatus.color;
    DirectX::XMMATRIX transMat = DirectX::XMMatrixTransformation2D(
        DirectX::XMVectorSet(0, 0, 0, 0), // 拡大縮小ピボットポイント
        0.0f, // 拡大縮小軸
        DirectX::XMVectorSet(initialStatus.size.x, initialStatus.size.y, 0, 0), // 拡大縮小
        DirectX::XMVectorSet(0, 0, 0, 0), // 回転ピボットポイント
        initialStatus.angle, // 回転角度
        DirectX::XMVectorSet(initialStatus.position.x + initialStatus.size.x / 2, initialStatus.position.y + initialStatus.size.y / 2, 0, 0) // 平行移動
    );

    for (auto action : actions)
    {
        // calculate value changes of offset / duration
        double actionProgress = (offset - durationApplied) / action.duration;
        durationApplied += min(actionProgress, 1) * action.duration;

        if (actionProgress > 1)
        {
            // this action already end
            transMat = transMat * action.trasnMat;
            color.w = min(color.w + action.opacity, 1);
        }
        else
        {
            // this action need calculate
            color.w = min(color.w + action.opacity * actionProgress, 1);
            transMat = transMat * (action.trasnMat * actionProgress);

            break;
        }
    }

    hal::dout << color.w << std::endl;

    float IMAGE_WIDTH = static_cast<float>(Texture_Width(textureId));
    float IMAGE_HEIGHT = static_cast<float>(Texture_Height(textureId));

    Sprite_Draw(textureId,
                initialStatus.position.x, initialStatus.position.y,
                0.0f, 0.0f,
                IMAGE_WIDTH, IMAGE_HEIGHT,
                initialStatus.size.x, initialStatus.size.y,
                transMat,
                color
    );
}

Sprite* Sprite::delay(double duration)
{
    totalDuration += duration;

    actions.emplace_back(duration, 0, DirectX::XMMatrixIdentity());

    return this;
}

Sprite* Sprite::fadeInOut(float from, float to, double duration)
{
    totalDuration += duration;

    actions.emplace_back(duration, to - from, DirectX::XMMatrixIdentity());

    return this;
}

Sprite* Sprite::move(DirectX::XMFLOAT2 from, DirectX::XMFLOAT2 to, double duration)
{
    totalDuration += duration;

    return this;
}
