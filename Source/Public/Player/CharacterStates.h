#pragma once

UENUM()
enum class ECharacterConfirmType : uint8
{
	Yes,
	No
};

UENUM()
enum class ECharacterValidType : uint8
{
	Valid,
	Invalid
};

UENUM()
enum class ECharacterSuccessType : uint8
{
	Success,
	Failed
};

UENUM()
enum class ECharacterCountdownActionInput : uint8
{
	Start,
	Cancel
};

UENUM()
enum class ECharacterCountdownActionOutput : uint8
{
	Updated,
	Completed,
	Canceled
};