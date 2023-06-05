// Fill out your copyright notice in the Description page of Project Settings.


#include "BloodComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Materials/MaterialInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/PrimitiveComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "RenderTargetPool.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Math/RandomStream.h"
#include "Math/Transform.h"

UBloodComponent::UBloodComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}
void UBloodComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	

}
void UBloodComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}







void UBloodComponent::Initialize(USkeletalMeshComponent* SkeletalMesh, USceneCaptureComponent2D* SceneCapture)
{
	if (SkeletalMesh && SceneCapture)
	{
		MeshReference = SkeletalMesh;
		SceneCaptureComponent2D = SceneCapture;
		bBloodMaskInitialized = true;



		SceneCaptureComponent2D->ProjectionType = ECameraProjectionMode::Orthographic;
		SceneCaptureComponent2D->OrthoWidth =1000.f;
		SceneCaptureComponent2D->CompositeMode = ESceneCaptureCompositeMode::SCCM_Composite;
		SceneCaptureComponent2D->bCaptureEveryFrame = false;
		SceneCaptureComponent2D->bCaptureOnMovement = false;
		TArray<FEngineShowFlagsSetting> ShowFlagSettings;
		ShowFlagSettings.AddDefaulted(1);
		ShowFlagSettings[0].ShowFlagName = FString("Skeletal Meshes");
		ShowFlagSettings[0].Enabled = true;
		SceneCaptureComponent2D->ShowFlagSettings = ShowFlagSettings;
	
	
		SceneCaptureComponent2D->SetRelativeTransform(Texture2DRelativeTransform ,false);



		GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);


		CalculateRefTransforms();



		BloodMask = NewObject<UTextureRenderTarget2D>(this);
		BloodMask->RenderTargetFormat = RTF_R8;
		BloodMask->InitAutoFormat(1024, 1024);
		BloodMask->bAutoGenerateMips = false;
		BloodMask->ClearColor = FLinearColor(0.0f, 0.0f, 0.0f, 1.0f);
		BloodMask->UpdateResourceImmediate(true);



		BloodMask_NotDilated = NewObject<UTextureRenderTarget2D>(this);
		BloodMask_NotDilated->RenderTargetFormat = RTF_R8;
		BloodMask_NotDilated->InitAutoFormat(1024, 1024);
		BloodMask_NotDilated->bAutoGenerateMips = false;
		BloodMask_NotDilated->ClearColor = FLinearColor(0.0f, 0.0f, 0.0f, 1.0f);
		BloodMask_NotDilated->UpdateResourceImmediate(true);


		SceneCaptureComponent2D->ShowOnlyActorComponents(GetOwner(), false);
		SceneCaptureComponent2D->TextureTarget = BloodMask_NotDilated;

		BloodRenderMaterial = UMaterialInstanceDynamic::Create(BloodRenderMaterialAsset,GetMesh());
		BloodRenderMaterial->SetTextureParameterValue(FName("BloodMask"), BloodMask_NotDilated);

		InitializeMaterials();

		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("RefPoseBoneTransform Length: %d,,Init Done"), GetMesh()->GetNumMaterials()));
	}
}





//ok
FTransform UBloodComponent::GetRefPoseBoneTransform(USkeletalMeshComponent* SkeletonMeshComponent, FName BoneName)
{
	//!!!!!!Attention!!!! Vector's * has order: A*B != B*A;
	FTransform BoneTransform;
	if (SkeletonMeshComponent != nullptr && !BoneName.IsNone())
	{
		{
			FTransform A = SkeletonMeshComponent->GetDeltaTransformFromRefPose(BoneName,FName("None")).Inverse();
			FTransform B = SkeletonMeshComponent->GetSocketTransform(BoneName, ERelativeTransformSpace::RTS_ParentBoneSpace);
			FTransform C = GetRefPoseBoneTransform(SkeletonMeshComponent, SkeletonMeshComponent->GetParentBone(BoneName));

			BoneTransform = (A * B) * C;

			return BoneTransform;
		}
	}
	else
	{
		FTransform A = SkeletonMeshComponent->GetDeltaTransformFromRefPose(BoneName,FName("None")).Inverse();
		FTransform B = SkeletonMeshComponent->GetSocketTransform(BoneName, ERelativeTransformSpace::RTS_World);

		BoneTransform = A * B;

		return BoneTransform;
	}
}


//ok
FVector UBloodComponent::TransformLocationByRefBoneTransform(FName BoneName, FVector WorldLocation)
{
	USkeletalMeshComponent* SKM = GetMesh();

	FVector A = UKismetMathLibrary::InverseTransformLocation(SKM->GetComponentTransform(), WorldLocation);

	FTransform B = SKM->GetSocketTransform(BoneName, ERelativeTransformSpace::RTS_Component).Inverse();

	FTransform C = RefPoseLocalBoneTransforms[SKM->GetBoneIndex(BoneName)];

	FVector OutVector = UKismetMathLibrary::
			TransformLocation((B * C) , A);
		
		
	return OutVector;
}
//ok
FVector UBloodComponent::TransformDirectionByRefBoneTransform(FName BoneName, FVector WorldLocation)
{
	USkeletalMeshComponent* SKM = GetMesh();

	FVector A = UKismetMathLibrary::InverseTransformDirection(SKM->GetComponentTransform(), WorldLocation);

	FTransform B = SKM->GetSocketTransform(BoneName, ERelativeTransformSpace::RTS_Component).Inverse();

	FTransform C = RefPoseLocalBoneTransforms[SKM->GetBoneIndex(BoneName)];

	FVector OutVector = UKismetMathLibrary::
		TransformDirection((B * C), A);


	return OutVector;
}


//ok
USkeletalMeshComponent* UBloodComponent::GetMesh()
{
	return MeshReference;
}


//ok
void UBloodComponent::CalculateRefTransforms()
{
	for (int i = 0 ;i < GetMesh()->GetNumBones()+1; i++)
	{
		RefPoseLocalBoneTransforms.Add(GetRefPoseBoneTransform(GetMesh(), GetMesh()->GetBoneName(i)));
	}

}

//ok
void UBloodComponent::InitializeMaterials()
{
	if (GetMesh())
	{
		for (int i = 0; i <= GetMesh()-> GetNumMaterials(); i++)
		{
			UMaterialInterface* MaterialInterface = GetMesh()->GetMaterial(i);
			if (MaterialInterface != nullptr)
			{
				UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(MaterialInterface, this);
				DynMaterial->SetTextureParameterValue(FName("BloodMask"), BloodMask_NotDilated);
				Materials.Add(DynMaterial);
				//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString("AddDynamicVision success"));
			}
			else if (MaterialInterface == nullptr)
			{
				Materials.Add(MaterialInterface);
				//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString("AddMIVision success"));
			}
		}
	;
	}

}


//ok
void UBloodComponent::TakeHit(FName BoneName, FVector WorldSpaceLocation)
{
	if (!bBloodMaskInitialized) 
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString("BloodComponent not initialized: At BeginPlay your character needs to call WoundComponent->InitializeMesh"));
		
		return ;

	}
	if (RefPoseLocalBoneTransforms.Num() <= 0)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString("BloodComponent could not find any bones in mesh"));
		return;
	}
	
	FVector Vector = TransformLocationByRefBoneTransform(BoneName, WorldSpaceLocation);
	
	
	FLinearColor VectorColor = FLinearColor(Vector.X, Vector.Y, Vector.Z, 1.f);

	BloodRenderMaterial->SetVectorParameterValue(FName("HitLocation"), VectorColor);


	float randomRadius = FMath::FRandRange(4.f, 10.f);
	BloodRenderMaterial->SetScalarParameterValue(FName("WoundRadius"), randomRadius);
	

	//ok
	for (int i = 0; i <= GetMesh()->GetNumMaterials(); i++) 
		{
			GetMesh()->SetMaterial(i, BloodRenderMaterial);
		}

	//ok
	SceneCaptureComponent2D->CaptureScene();
	



	//ok
	for (int j = 0; j <= GetMesh()->GetNumMaterials() ; j++)
		{
			int s = GetMesh()->GetNumMaterials();		
			GetMesh()->SetMaterial(j, Materials[j]);
			//GEngine->AddOnScreenDebugMessage(-1, 101.f, FColor::Red, FString::Printf(TEXT("GetNumMaterials: %d"), j));
		}

	//GEngine->AddOnScreenDebugMessage(-1, 101.f, FColor::Red, FString::Printf(TEXT("GetNumMaterials: %d"), GetMesh()->GetNumMaterials()));
	//FTransform BoneTransform = GetRefPoseBoneTransform(GetMesh(), BoneName);
	//FVector BoneTransform2 = TransformLocationByRefBoneTransform(FName("head"), FVector(0.f, 0.f, 0.f));
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Bone Value: %s"), *BoneTransform2.ToString()));

}

