AAdventureGameMode::AAdventureGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
		PrimaryActorTick.bCanEverTick = true;
	}
}

void AAdventureGameMode::BeginPlay()
{
	Super::BeginPlay();

	Player = GetWorld()->GetFirstPlayerController()->GetPawn();

	//Spawn First Straight
	PositionY -= 3000.f;
	FTransform Transform(FVector(PositionX, PositionY, 130.f));
	CurrFragment = GetWorld()->SpawnActor<AFragmentForward>(FragmentForward_BP, Transform);
	//Fragments.Add(CurrFragment);
	
	//Spawn Next Random
	SpawnNext();
}

void AAdventureGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (Player->IsOverlappingActor(CurrFragment)) 
	{
		SpawnNext();
	}
}

void AAdventureGameMode::SpawnNext()
{
	DestroyPrev();
	PrevFragment = CurrFragment ? CurrFragment : NULL;
	int Direction = FMath::RandRange(0, 2);
	Positions.Add(Direction);

	switch (Direction)
	{
	case 0:
		CurrFragment = GetWorld()->SpawnActor<AFragmentForward>(FragmentForward_BP, CalcTransform());
		break;
	case 1:
		CurrFragment = GetWorld()->SpawnActor<AFragmentRight>(FragmentRight_BP, CalcTransform());
		++Orientation %= 4;
		break;
	case 2:
		CurrFragment = GetWorld()->SpawnActor<AFragmentLeft>(FragmentLeft_BP, CalcTransform());;
		Orientation = --Orientation < 0 ? 3 : Orientation;
		break;
	}
	CurrFragmentIndex++;
	UE_LOG(LogTemp, Warning, TEXT("%s"), *CurrFragment->GetName())
}

void AAdventureGameMode::DestroyPrev()
{
	if (PrevFragment)
	{
		PrevFragment->Destroy();
	}
}

FTransform AAdventureGameMode::CalcTransform()
{
	switch (Orientation)
	{
	case 0:
		PositionY -= 3000.f;
		break; 
	case 1:
		PositionX += 3000.f;
		break;
	case 2:
		PositionY += 3000.f;
		break;
	case 3:
		PositionX -= 3000.f;
		break;
	}
	FTransform Transform(FVector(PositionX, PositionY, 130.f));

	switch (Positions[CurrFragmentIndex])
	{
	case 0:
		Transform.SetRotation(FQuat(FRotator(0.f, ForwardRotator * 90.f, 0.f)));
		break;
	case 1:
		Transform.SetRotation(FQuat(FRotator(0.f, Orientation * 90.f, 0.f)));
		ForwardRotator = !ForwardRotator;
		//Right Rotator
		SideRotator = --SideRotator < 0 ? 3 : SideRotator;
		break;
	case 2:
		Transform.SetRotation(FQuat(FRotator(0.f, Orientation * -90.f, 0.f)));
		ForwardRotator = !ForwardRotator;
		//Left Rotator
		++SideRotator %= 4;
		break;
	}
	return Transform;
}