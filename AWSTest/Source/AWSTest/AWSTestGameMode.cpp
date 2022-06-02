// Copyright Epic Games, Inc. All Rights Reserved.

#include "AWSTestGameMode.h"
#include "AWSTest.h"
#include "Engine.h"
#include "EngineGlobals.h"
#include "AWSTestHUD.h"
#include "AWSTestCharacter.h"
#include "GameLiftServerSDK.h"
#include "UObject/ConstructorHelpers.h"

AAWSTestGameMode::AAWSTestGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AAWSTestHUD::StaticClass();
	GameLiftInit();
}

void AAWSTestGameMode::GameLiftInit()
{
	// ServerTarget����GameLift���L���ȏꍇ�̂ݗL��
#if WITH_GAMELIFT
	
	// �ŏ���GameLiftModule���擾����
	FGameLiftServerSDKModule* gameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));

	// GameLift�Ƃ̃��[�J���ڑ����m�����A�ʐM���\��Ԃɂ���
	gameLiftSdkModule->InitSDK();

	// �V�����Q�[���Z�b�V�����̃A�N�e�B�x�[�V�����v���ɉ�������
	// �Q�[���T�[�o�[�ɃQ�[���v���p�e�B���܂ރQ�[���Z�b�V�����I�u�W�F�N�g�y�уT�[�o�[�N���v���𑗐M����
	// �Q�[���T�[�o�[���v���C���[�̐ڑ����󂯓���鏀�����o������ GameLiftAPI.Session ���Ăяo��
	
	auto onGameSession = [=](Aws::GameLift::Server::Model::GameSession gameSession)
	{
		gameLiftSdkModule->ActivateGameSession();
	};

	FProcessParameters* params = new FProcessParameters();
	params->OnStartGameSession.BindLambda(onGameSession);

	// OnProcessTerminate�R�[���o�b�N�BGameLift�͂��̃C���X�^���X���V���b�g�_�E������O�ɌĂяo��

	// �P��GameLift���V���b�g�_�E�����邱�Ƃ�`���Ă��邾��
	auto terminateGameSession = [=]()
	{
		gameLiftSdkModule->ProcessEnding();
	};
	params->OnTerminate.BindLambda(terminateGameSession);

	// HealthCheck�R�[���o�b�N�BGameLift�͖�60�b���Ƃɂ��̃R�[���o�b�N���Ăяo���B
	// �f�t�H���g�ł͎����I��true��Ԃ��B�Q�[���̓I�v�V�����ňȉ��̃`�F�b�N���s�����Ƃ��ł���炵��
	// �ˑ��֌W�̃`�F�b�N���s���A���̏��Ɋ�Â��ăX�e�[�^�X��񍐂��邱�Ƃ��ł���B60�b�ȓ��ɉ������Ȃ��ꍇfalse���L�^�����
	// �Ȃ�����͏��true
	auto healthCheck = []()
	{
		return true;
	};
	params->OnHealthCheck.BindLambda(healthCheck);

	// GameLift�ւ�port�ԍ��̒ʒm
	params->port = 7777;

	// GameLift�ɑ΂��āA�Q�[���Z�b�V�������I�������Ƃ��ɃA�b�v���[�h���ׂ��t�@�C���Q�̎w��
	TArray<FString> logfiles;
	logfiles.Add(TEXT("aLogFile.txt"));
	params->logParameters = logfiles;

	// ProcessReady���Ăяo����GameLift�ɂ��̃Q�[���T�[�o�[�����������Ȃ��Ƃ�`����
	gameLiftSdkModule->ProcessReady(*params);
#endif
}
