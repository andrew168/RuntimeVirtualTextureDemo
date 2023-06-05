// Fill out your copyright notice in the Description page of Project Settings.


#include "BloodCharacter.h"
#include "BloodComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Blueprint/BlueprintSupport.h"

// Sets default values
ABloodCharacter::ABloodCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BloodComponent = CreateDefaultSubobject<UBloodComponent>(TEXT("BloodComponent"));
	SceneCapture2D = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture2D"));
	SceneCapture2D->ProjectionType = ECameraProjectionMode::Perspective;
	SceneCapture2D->CompositeMode = ESceneCaptureCompositeMode::SCCM_Overwrite;
	SceneCapture2D->bCaptureEveryFrame = false;
	SceneCapture2D->bCaptureOnMovement = false;
	SceneCapture2D->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale);
	FTransform Transform(FRotator(180.0f, -90.0f, 90.0f), FVector(0.0f, 0.0f, 250.0f), FVector(1.0f, 1.0f, 1.0f));
	SceneCapture2D->SetRelativeTransform(Transform);
	

	static const FName IsDataOnly("false");
}

void ABloodCharacter::InitializeBloodComponent()
{
	BloodComponent->Initialize(GetMesh(), SceneCapture2D);
}

float ABloodCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = DamageAmount;
    Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    UPrimitiveComponent* HitComponent = nullptr;

    if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))//if damage type is point damage,call Takehit.pass the paramater to the Bloodcomponent;
    {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString("OK"));
        const FPointDamageEvent& PointDamageEvent = static_cast<const FPointDamageEvent&>(DamageEvent);
        HitComponent = PointDamageEvent.HitInfo.GetActor()->FindComponentByClass<UPrimitiveComponent>();
		FName BoneName = PointDamageEvent.HitInfo.BoneName;
		FVector HitLocation = PointDamageEvent.HitInfo.Location;
		if (HitComponent == GetMesh())
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString("O22K"));
			BloodComponent->TakeHit(BoneName, HitLocation);
		}
    }


   
	return ActualDamage;
}

// Called when the game starts or when spawned
void ABloodCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeBloodComponent();


}

// Called every frame
void ABloodCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABloodCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

