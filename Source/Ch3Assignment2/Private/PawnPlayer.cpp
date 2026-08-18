// Fill out your copyright notice in the Description page of Project Settings.


#include "PawnPlayer.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"

#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"


// Sets default values
APawnPlayer::APawnPlayer()
{
	PrimaryActorTick.bCanEverTick = true;
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	SetRootComponent(CapsuleComp);
	CapsuleComp->InitCapsuleSize(42.0f, 96.0f);
	CapsuleComp->SetSimulatePhysics(false);
	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComp"));
	SkeletalMeshComp->SetupAttachment(CapsuleComp);
	SkeletalMeshComp->SetSimulatePhysics(false);


	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(CapsuleComp);
	SpringArmComp->TargetArmLength = 300.0f;
	SpringArmComp->SetRelativeLocation(FVector(0.0f, 0.0f, 60.0f));
	SpringArmComp->bUsePawnControlRotation = false;
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp,USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;
}

// Called when the game starts or when spawned
void APawnPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APawnPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!MoveInput.IsNearlyZero())
	{
		FVector Forward = GetActorForwardVector();
		FVector Right = GetActorRightVector();

		Forward.Z = 0.0f;
		Right.Z = 0.0f;
		Forward.Normalize();
		Right.Normalize();

		FVector MoveDirection = Forward * MoveInput.Y + Right * MoveInput.X;
		MoveDirection.Normalize();
		FVector MoveAmount = MoveDirection * MoveSpeed * DeltaTime;

		AddActorWorldOffset(MoveAmount,true);
	}
	if (!LookInput.IsNearlyZero())
	{
		float YawAmount =LookInput.X * LookSpeed * DeltaTime;
		FRotator YawRotation(0.0f,YawAmount,0.0f);
		AddActorLocalRotation(YawRotation);
		float PitchAmount = -LookInput.Y * LookSpeed * DeltaTime;
		CameraPitch += PitchAmount;
		CameraPitch = FMath::Clamp(CameraPitch,MinPitch,MaxPitch);
		SpringArmComp->SetRelativeRotation(FRotator(CameraPitch,0.0f,0.0f));
	}
}

void APawnPlayer::Move(const FInputActionValue& Value)
{
	MoveInput = Value.Get<FVector2D>();
}


void APawnPlayer::StopMove(const FInputActionValue& Value)
{
	MoveInput = FVector2D::ZeroVector;
}


void APawnPlayer::Look(const FInputActionValue& Value)
{
	LookInput = Value.Get<FVector2D>();
}


void APawnPlayer::StopLook(const FInputActionValue& Value)
{
	LookInput = FVector2D::ZeroVector;
}

void APawnPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();

		if (LocalPlayer)
		{
			UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
			if (Subsystem && InputMappingContext)
			{
				if (!Subsystem->HasMappingContext(InputMappingContext))
				{
					Subsystem->AddMappingContext(InputMappingContext,0);
				}
			}
		}
	}
	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInput)
	{
		if (MoveAction)
		{
			EnhancedInput->BindAction(MoveAction,ETriggerEvent::Triggered,this,&APawnPlayer::Move);
			EnhancedInput->BindAction(MoveAction,ETriggerEvent::Completed,this,&APawnPlayer::StopMove);
			EnhancedInput->BindAction(MoveAction,ETriggerEvent::Canceled,this,&APawnPlayer::StopMove);
		}


		if (LookAction)
		{
			EnhancedInput->BindAction(LookAction,ETriggerEvent::Triggered,this,&APawnPlayer::Look);
			EnhancedInput->BindAction(LookAction,ETriggerEvent::Completed,this,&APawnPlayer::StopLook);
			EnhancedInput->BindAction(LookAction,ETriggerEvent::Canceled,this,&APawnPlayer::StopLook);
		}
	}
}

