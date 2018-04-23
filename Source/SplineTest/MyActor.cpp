// Fill out your copyright notice in the Description page of Project Settings.

#include "MyActor.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"

// Ref: http://api.unrealengine.com/INT/API/Runtime/Engine/DrawDebugLine/index.html
// Ref: https://wiki.unrealengine.com/Draw_3D_Debug_Points,_Lines,_and_Spheres:_Visualize_Your_Algorithm_in_Action

// Sets default values
AMyActor::AMyActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;



	p[0].Set(0, 0, 150);
	p[1].Set(0, 150, 150);
	p[2].Set(150, 0, 200);
	p[3].Set(150, -100, 150);
	p[4].Set(0, 200, 300);
	p[5].Set(-150, 100, 150);
	p[6].Set(-150, 300, 100);
	p[7].Set(-250, 200, 150);
	p[8].Set(-150, 500, 150);
	p[9].Set(-350, 100, 150);

	t[0] = -1.f;
	for (int i = 1; i < num_keys; i++)
	{
		t[i] = t[i - 1] + 1.f;// FMath::RandRange(0.5f, 1.5f);
	}



	for (int i = 1; i < num_keys; i++)
	{
		q[i] = FQuat(FRotator(i*i*20, i*i* 30, i * 40));
	}
}

// Called when the game starts or when spawned
void AMyActor::BeginPlay()
{
	Super::BeginPlay();

	// Key Points
	for (int i = 0; i < num_keys; i++)
	{
		DrawDebugBox(
			GetWorld(),
			p[i],
			FVector(5, 5, 5),
			q[i],
			FColor(255, 0, 255),  //pink
			true//false,  				//persistent (never goes away)
			//0.03 					//point leaves a trail on moving object
		);
	}

	// Lines
	for (int i = 1; i < num_keys - 2; i++)
	{
		DrawDebugLine(
			GetWorld(),
			p[i],
			p[i + 1],
			FColor(255, 255, 0),
			true
		);
	}

	// CR Spline
	for (int i = 1; i < num_keys - 2; i++)
	{
		int i0 = i - 1;
		int i1 = i;
		int i2 = i + 1;
		int i3 = i + 2;

		for (float j = 0.0f; j < 1.0f; j += 0.1f)
		{
			FVector s0 = FMath::CubicCRSplineInterpSafe(p[i0], p[i1], p[i2], p[i3], t[i0], t[i1], t[i2], t[i3], t[i1] + j);
			FVector s1 = FMath::CubicCRSplineInterpSafe(p[i0], p[i1], p[i2], p[i3], t[i0], t[i1], t[i2], t[i3], t[i1] + j + 0.1f);
			DrawDebugLine(
				GetWorld(),
				s0,
				s1,
				FColor(0, 255, 0),
				true//, false
				//, 0.03
			);
		}
	}
}

// Called every frame
void AMyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	

	float gt = GetGameTimeSinceCreation();

	// Linear Interpolation
	if ( false )
	{
		

		int sec = (int)gt;
		float w = gt - (float)sec;

		sec = sec % (num_keys - 1);


		FVector pp;

		pp = FMath::Lerp(p[sec], p[sec + 1], w);
		DrawDebugPoint(
			GetWorld(),
			pp,
			20,  					//size
			FColor(255, 0, 0),  //pink
			false,  				//persistent (never goes away)
			0.03 					//point leaves a trail on moving object
		);
	}


	// CR
	if (true)
	{
		int c_i = -1;

		if (gt < t[1])
		{
			c_i = 1;
			gt = t[1];
		}

		for (int i = 1; i < num_keys - 2; i++)
		{
			if (t[i] < gt && gt < t[i + 1])
			{
				c_i = i;
				break;
			}
		}

		if (c_i == -1)
		{
			c_i = num_keys - 3;
			gt = t[num_keys-2];
		}

		FVector ps;
		ps = FMath::CubicCRSplineInterpSafe(

			p[c_i-1], 
			p[c_i], 
			p[c_i+1], 
			p[c_i+2], 

			t[c_i-1],
			t[c_i],
			t[c_i + 1],
			t[c_i + 2],

			gt
		);

		FQuat qs;
		qs = FMath::CubicCRSplineInterpSafe(

			q[c_i - 1],
			q[c_i],
			q[c_i + 1],
			q[c_i + 2],

			t[c_i - 1],
			t[c_i],
			t[c_i + 1],
			t[c_i + 2],

			gt
		);

		DrawDebugBox(
			GetWorld(),
			ps,
			FVector(10, 10, 10),
			qs,
			FColor(255, 0, 0),  //pink
			false,  				//persistent (never goes away)
			0.1 					//point leaves a trail on moving object
		);

		// this->GetComponents()[0]

		SetActorLocationAndRotation(ps, qs);
	}
}

