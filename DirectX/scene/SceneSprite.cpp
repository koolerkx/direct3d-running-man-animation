#include "SceneSprite.h"

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

void Sprite::draw(int offset)
{
    Sprite_Draw(textureId, 512.0f, 32.0f, 256, 256);
}
