// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PawnExtensionComponentBase.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CRYPTRAIDER_API UPawnExtensionComponentBase : public UActorComponent
{
	GENERATED_BODY()

protected:
	// Templated function of type t that uses assert to verify that T is indeed derived from APawn class, otherwise it will fail to compile with shown message
	// Then performs cast checked to make sure cast succeeds to type T and gets the owner
	template<class T>
	T* GetOwningPawn() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, APawn>::Value, "'T' Template parameter to GetPawn must be derived from APawn.");
		return CastChecked<T>(GetOwner());
	}

	APawn* GetOwningPawn() const
	{
		return GetOwningPawn<APawn>();
	}

	template<class T>
	T* GetOwningController() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, AController>::Value, "'T' Template parameter to GetOwningController must be derived from AController.");
		return GetOwningPawn<APawn>()->GetController<T>();
	}
};
