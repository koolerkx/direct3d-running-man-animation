/**
 * @file texture.cpp
 * @brief テクスチャ管理
 * @author KOOLER FAN
 * @date 2025/06/12
 */

#include "texture.h"

#include "direct3d.h"
#include "DirectXTex.h"
using namespace DirectX;
#include <string>

static constexpr int TEXTURE_MAX = 256; ///< 最大管理数

// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

/**
* @struct Texture
* @brief Texture が管理するもの
*/
struct Texture
{
    std::wstring filename;
    ID3D11ShaderResourceView* pTexture;
    unsigned int width;
    unsigned int height;
};

static Texture g_Textures[TEXTURE_MAX]{};
static int g_SetTextureIndex = -1;

/**
* @pre Direct3D_Initialize() Shader_Initialize() の後で使う 
*/
void Texture_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    // 念のため、全部初期化する
    for (Texture& t : g_Textures)
    {
        t.pTexture = nullptr;
    }

    g_SetTextureIndex = -1;

    // デバイスとデバイスコンテキストの保存
    g_pDevice = pDevice;
    g_pContext = pContext;
}

void Texture_Finalize(void)
{
    Texture_AllRelease();
}

int Texture_Load(const wchar_t* pFilename)
{
    // すでに読み込んだファイルは読み込まない
    for (int i = 0; i < TEXTURE_MAX; i++)
    {
        if (g_Textures[i].filename == pFilename)
        {
            return i;
        }
    }

    // 空いている管理領域を探す
    for (int i = 0; i < TEXTURE_MAX; i++)
    {
        if (g_Textures[i].pTexture) continue; // 使用中なら次のスロットを探す

        // テクスチャの読み込み
        TexMetadata metadata;
        ScratchImage image;

        HRESULT hr = LoadFromWICFile(pFilename, WIC_FLAGS_NONE, &metadata, image);
        if (FAILED(hr))
        {
            MessageBoxW(nullptr, L"テクスチャの初期化に失敗しました", pFilename, MB_OK);
            return -1;
        }

        g_Textures[i].filename = pFilename;
        g_Textures[i].width = static_cast<unsigned int>(metadata.width);
        g_Textures[i].height = static_cast<unsigned int>(metadata.height);
        hr = CreateShaderResourceView(
            g_pDevice,
            image.GetImages(),
            image.GetImageCount(),
            metadata,
            &g_Textures[i].pTexture
        );

        return i;
    }

    return -1;
}

void Texture_AllRelease()
{
    for (Texture& t : g_Textures)
    {
        t.filename.clear();
        SAFE_RELEASE(t.pTexture);
    }
}

void Texture_SetTexture(int texid)
{
    if (texid < 0) return;
    // if (g_SetTextureIndex == texid) return; // すでに、ShaderResourcesに設定したから

    g_SetTextureIndex = texid;

    // テクスチャ設定
    g_pContext->PSSetShaderResources(0, 1, &g_Textures[texid].pTexture);
}

int Texture_Width(int texid)
{
    if (texid < 0) return -1;

    return g_Textures[texid].width;
}

int Texture_Height(int texid)
{
    if (texid < 0) return -1;

    return g_Textures[texid].height;
}
