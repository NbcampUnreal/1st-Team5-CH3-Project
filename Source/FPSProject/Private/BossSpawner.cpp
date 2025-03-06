#include "BossSpawner.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "BossAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

// 생성자
ABossSpawner::ABossSpawner()
{
    // Tick 함수 활성화
    PrimaryActorTick.bCanEverTick = true;
    
    // 기본값 설정
    SpawnDelay = 10.0f; // 컴뱃 페이즈 시작 후 10초 후에 스폰
    SpawnOffset = FVector(0.0f, 0.0f, 100.0f);
    bHasSpawnedBoss = false; // 보스 스폰 여부 초기화
    bHasDetectedCombatPhase = false; // 컴뱃 페이즈 감지 여부 초기화
    PreviousGamePhase = EGamePhase::Tutorial; // 이전 게임 페이즈 초기화
    SpawnedBoss = nullptr;
    CachedGameState = nullptr;
}

// 게임 시작 시 호출
void ABossSpawner::BeginPlay()
{
    Super::BeginPlay();
    
    // 게임 상태 캐싱
    CachedGameState = Cast<ABasicGameState>(UGameplayStatics::GetGameState(GetWorld()));
    
    if (CachedGameState)
    {
        // 현재 게임 페이즈 확인
        PreviousGamePhase = CachedGameState->CurrentPhase;
        
        // 이미 컴뱃 페이즈인 경우 타이머 설정
        if (CachedGameState->CurrentPhase == EGamePhase::Combat)
        {
            bHasDetectedCombatPhase = true;
            
            // 타이머 설정
            FTimerDelegate TimerDelegate;
            TimerDelegate.BindUObject(this, &ABossSpawner::SpawnBossForTimer);
            
            GetWorld()->GetTimerManager().SetTimer(
                SpawnTimerHandle,
                TimerDelegate,
                SpawnDelay,
                false);
                
            UE_LOG(LogTemp, Warning, TEXT("컴뱃 페이즈 감지! %f초 후에 보스가 스폰됩니다."), SpawnDelay);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("게임 상태를 찾을 수 없습니다."));
    }
}

// 매 프레임 호출
void ABossSpawner::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // 이미 보스를 스폰했거나 컴뱃 페이즈를 감지했으면 무시
    if (bHasSpawnedBoss || bHasDetectedCombatPhase)
    {
        return;
    }
    
    // 게임 상태 확인
    if (!CachedGameState)
    {
        CachedGameState = Cast<ABasicGameState>(UGameplayStatics::GetGameState(GetWorld()));
        if (!CachedGameState)
        {
            return;
        }
    }
    
    // 현재 게임 페이즈가 컴뱃 페이즈로 변경되었는지 확인
    if (CachedGameState->CurrentPhase == EGamePhase::Combat && PreviousGamePhase != EGamePhase::Combat)
    {
        bHasDetectedCombatPhase = true;
        
        // 타이머 설정
        FTimerDelegate TimerDelegate;
        TimerDelegate.BindUObject(this, &ABossSpawner::SpawnBossForTimer);
        
        GetWorld()->GetTimerManager().SetTimer(
            SpawnTimerHandle,
            TimerDelegate,
            SpawnDelay,
            false);
            
        UE_LOG(LogTemp, Warning, TEXT("컴뱃 페이즈 감지! %f초 후에 보스가 스폰됩니다."), SpawnDelay);
    }
    
    // 이전 게임 페이즈 업데이트
    PreviousGamePhase = CachedGameState->CurrentPhase;
}

// 보스 스폰 함수
ABossCharacter* ABossSpawner::SpawnBoss()
{
    // 이미 보스를 스폰했으면 그대로 반환
    if (bHasSpawnedBoss)
    {
        UE_LOG(LogTemp, Warning, TEXT("이미 보스를 스폰했습니다."));
        return SpawnedBoss;
    }
    
    // 보스 클래스가 설정되어 있는지 확인
    if (!BossClass)
    {
        UE_LOG(LogTemp, Error, TEXT("보스 클래스가 설정되지 않았습니다."));
        return nullptr;
    }
    
    // 스폰 위치 계산
    FVector SpawnLocation = GetActorLocation() + SpawnOffset;
    FRotator SpawnRotation = GetActorRotation();
    
    // 스폰 파라미터 설정
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    
    // 보스 스폰
    SpawnedBoss = GetWorld()->SpawnActor<ABossCharacter>(
        BossClass,
        SpawnLocation,
        SpawnRotation,
        SpawnParams);
        
    if (SpawnedBoss)
    {
        // 보스 스폰 성공 표시
        bHasSpawnedBoss = true;
        
        UE_LOG(LogTemp, Warning, TEXT("보스가 성공적으로 스폰되었습니다."));
        
        // 보스가 스폰되면 즉시 활성화 (Sleep 상태 해제)
        if (SpawnedBoss->IsSleeping())
        {
            // Sleep 상태에서 깨우기
            SpawnedBoss->WakeUp();
            
            // 보스 AI 컨트롤러 활성화
            ABossAIController* BossAI = Cast<ABossAIController>(SpawnedBoss->GetController());
            if (BossAI)
            {
                BossAI->GetBrainComponent()->RestartLogic();
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("보스 스폰에 실패했습니다."));
    }
    
    return SpawnedBoss;
}

// 타이머에서 호출할 보스 스폰 함수 (void 반환)
void ABossSpawner::SpawnBossForTimer()
{
    SpawnBoss();
} 