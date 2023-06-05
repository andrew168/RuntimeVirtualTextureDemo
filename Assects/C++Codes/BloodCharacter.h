// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BloodCharacter.generated.h"

UCLASS()
class BLOOD_CH_API ABloodCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABloodCharacter();


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
		class UBloodComponent* BloodComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
		class USceneCaptureComponent2D* SceneCapture2D;


	UFUNCTION(BlueprintCallable)
		void InitializeBloodComponent();



	virtual float TakeDamage
	(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser
	);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
