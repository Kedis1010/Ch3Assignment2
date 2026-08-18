// Fill out your copyright notice in the Description page of Project Settings.


#include "SpartaGameMode.h"
#include "PawnPlayer.h"


ASpartaGameMode::ASpartaGameMode()
{
	DefaultPawnClass = APawnPlayer::StaticClass();
}