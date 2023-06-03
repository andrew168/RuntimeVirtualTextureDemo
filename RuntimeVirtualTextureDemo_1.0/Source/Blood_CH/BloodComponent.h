// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BloodComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLOOD_CH_API UBloodComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;






public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTransform Texture2DRelativeTransform;

	TArray<FTransform> RefPoseLocalBoneTransforms;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
		class UTextureRenderTarget2D* BloodMask;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UTextureRenderTarget2D* BloodMask_NotDilated;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USceneCaptureComponent2D* SceneCaptureComponent2D;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USkeletalMeshComponent* MeshReference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UMaterialInstanceDynamic* BloodRenderMaterial;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class UMaterialInterface*> Materials;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bBloodMaskInitialized = false;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UMaterialInterface* BloodRenderMaterialAsset;
	

public:	
	UBloodComponent();

	UFUNCTION(BlueprintCallable)
		void Initialize(USkeletalMeshComponent* SkeletalMesh,USceneCaptureComponent2D* SceneCapture);


	UFUNCTION(BlueprintCallable)
		FTransform GetRefPoseBoneTransform(USkeletalMeshComponent* SkeletonMeshComponent, FName BoneName);



	UFUNCTION(BlueprintCallable)
		FVector TransformLocationByRefBoneTransform(FName BoneName,FVector WorldLocation);


	UFUNCTION(BlueprintCallable)
		FVector TransformDirectionByRefBoneTransform(FName BoneName, FVector WorldLocation);


	UFUNCTION(BlueprintCallable)
		USkeletalMeshComponent* GetMesh();

	UFUNCTION(BlueprintCallable)
		void CalculateRefTransforms();

	UFUNCTION(BlueprintCallable)
		void InitializeMaterials();

	UFUNCTION(BlueprintCallable)
		void TakeHit(FName BoneName, FVector WorldSpaceLocation);

};
