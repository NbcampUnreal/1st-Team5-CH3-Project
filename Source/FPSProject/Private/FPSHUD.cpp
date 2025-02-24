// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSHUD.h"
#include "Engine/Canvas.h" 

void AFPSHUD::DrawHUD()
{
    Super::DrawHUD();

    if (CrosshairTexture)
    {
        // ȭ�� �߾� ��ǥ ���
        float CenterX = Canvas->ClipX * 0.5f;
        float CenterY = Canvas->ClipY * 0.57f;

        // ũ�ν���� ũ��
        float TextureWidth = CrosshairTexture->GetSizeX();
        float TextureHeight = CrosshairTexture->GetSizeY();

        // ũ�ν���� ��ġ ����
        float DrawX = CenterX - (TextureWidth * 0.5f);
        float DrawY = CenterY - (TextureHeight * 0.5f);

        // ĵ������ ũ�ν���� �ؽ�ó�� �׸�
        DrawTexture(CrosshairTexture, DrawX, DrawY, TextureWidth, TextureHeight, 0, 0, 1, 1);
    }
}