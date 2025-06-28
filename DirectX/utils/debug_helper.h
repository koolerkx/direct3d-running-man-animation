#pragma once

#include <debug_ostream.h>
#include <DirectXMath.h>

using namespace DirectX;

inline void PrintMatrix(const XMMATRIX& m)
{
    XMFLOAT4X4 mat;
    XMStoreFloat4x4(&mat, m);

    hal::dout << "XMMATRIX:" << std::endl;
    for (int i = 0; i < 4; ++i)
    {
        hal::dout << "[ ";
        for (int j = 0; j < 4; ++j)
        {
            hal::dout << mat.m[i][j] << " ";
        }
        hal::dout << "]" << std::endl;
    }
}

inline void PrintXMFLOAT4(const XMFLOAT4& v)
{
    hal::dout << "XMFLOAT4: ("
        << v.x << ", "
        << v.y << ", "
        << v.z << ", "
        << v.w << ")"
        << std::endl;
}

inline void PrintXMFLOAT2(const XMFLOAT2& v)
{
    hal::dout << "XMFLOAT2: ("
        << v.x << ", "
        << v.y << ")"
        << std::endl;
}

inline void PrintSpriteState(const SpriteState& s)
{
    hal::dout << "SpriteState:" << std::endl;
    hal::dout << "  size: (" << s.size.x << ", " << s.size.y << ")" << std::endl;
    hal::dout << "  position: (" << s.position.x << ", " << s.position.y << ")" << std::endl;
    hal::dout << "  scale: (" << s.scale.x << ", " << s.scale.y << ")" << std::endl;
    hal::dout << "  rotation: " << s.rotation << std::endl;
    hal::dout << "  color: (" << s.color.x << ", " << s.color.y << ", " << s.color.z << ", " << s.color.w << ")" << std::endl;
}