// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSHUD.h"
#include "Engine/Canvas.h" 

void AFPSHUD::DrawHUD()
{
    Super::DrawHUD();

    if (CrosshairTexture)
    {
        // 화면 중앙 좌표 계산
        float CenterX = Canvas->ClipX * 0.5f;
        float CenterY = Canvas->ClipY * 0.55f;

        // 크로스헤어 크기
        float TextureWidth = CrosshairTexture->GetSizeX();
        float TextureHeight = CrosshairTexture->GetSizeY();

        // 크로스헤어 위치 설정
        float DrawX = CenterX - (TextureWidth * 0.5f);
        float DrawY = CenterY - (TextureHeight * 0.5f);

        // 캔버스에 크로스헤어 텍스처를 그림
        DrawTexture(CrosshairTexture, DrawX, DrawY, TextureWidth, TextureHeight, 0, 0, 1, 1);
    }
}
