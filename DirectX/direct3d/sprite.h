/**
 * @file sprite.h
 * @brief スプライト表示
 * @author KOOLER FAN
 * @date 2025/06/12
 */

#ifndef SPRITE_H
#define SPRITE_H

#include <d3d11.h>
#include <DirectXMath.h>

void Sprite_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

void Sprite_Finalize(void);

void Sprite_Begin();

/**
 * @brief テクスチャ全表示（表示サイズ変更可）
 * @param texid テクスチャID
 * @param display_x 左上のX座標
 * @param display_y 左上のY座標
 * @param angle 回転角度
 * @param color 色
 */
void Sprite_Draw(int texid, float display_x, float display_y, float angle = 0,
                 const DirectX::XMFLOAT4& color = {1.0f, 1.0f, 1.0f, 1.0f});

/**
 * @brief テクスチャ全表示（表示サイズ変更可）
 * @param texid テクスチャID
 * @param display_x 左上のX座標
 * @param display_y 左上のY座標
 * @param display_w 表示幅
 * @param display_h 表示高さ
 * @param angle 回転角度
 * @param color 色
 */
void Sprite_Draw(int texid, float display_x, float display_y, float display_w, float display_h, float angle = 0,
                 const DirectX::XMFLOAT4& color = {1.0f, 1.0f, 1.0f, 1.0f});

/**
 * @brief UVカット
 * @param texid テクスチャID
 * @param display_x 左上のX座標
 * @param display_y 左上のY座標
 * @param uvcut_x UVカット左上X座標
 * @param uvcut_y UVカット左上Y座標
 * @param uvcut_w UVカットの幅
 * @param uvcut_h UVカットの高さ
 * @param angle 回転角度
 * @param color 色
 */
void Sprite_Draw(int texid, float display_x, float display_y, float uvcut_x, float uvcut_y, float uvcut_w,
                 float uvcut_h, float angle = 0, const DirectX::XMFLOAT4& color = {1.0f, 1.0f, 1.0f, 1.0f});

/**
 * @brief UVカット（表示サイズ変更可）
 * @param texid テクスチャID
 * @param display_x 左上のX座標
 * @param display_y 左上のY座標
 * @param uvcut_x UVカット左上X座標
 * @param uvcut_y UVカット左上Y座標
 * @param uvcut_w UVカットの幅
 * @param uvcut_h UVカットの高さ
 * @param display_w 表示幅
 * @param display_h 表示高さ
 * @param angle 回転角度
 * @param color 色
 */
void Sprite_Draw(int texid, float display_x, float display_y, float uvcut_x, float uvcut_y, float uvcut_w,
                 float uvcut_h, float display_w, float display_h,
                 float angle = 0,
                 const DirectX::XMFLOAT4& color = {1.0f, 1.0f, 1.0f, 1.0f});

/**
 * @brief UVカット（表示サイズ変更可）
 * @param texid テクスチャID
 * @param display_x 左上のX座標
 * @param display_y 左上のY座標
 * @param uvcut_x UVカット左上X座標
 * @param uvcut_y UVカット左上Y座標
 * @param uvcut_w UVカットの幅
 * @param uvcut_h UVカットの高さ
 * @param display_w 表示幅
 * @param display_h 表示高さ
 * @param mat 変換行列
 * @param color 色
 */
void Sprite_Draw(int texid, float display_x, float display_y, float uvcut_x, float uvcut_y, float uvcut_w,
                 float uvcut_h, float display_w, float display_h,
                 DirectX::XMMATRIX mat,
                 const DirectX::XMFLOAT4& color = {1.0f, 1.0f, 1.0f, 1.0f});

#endif // SPRITE_H
